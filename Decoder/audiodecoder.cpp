#include <QDebug>

#include "audiodecoder.h"
#include "utils.h"

#define MAX_QUEUE_BYTES 64 * 1024 // 64 KB（≈ 166 ms）

AudioDecoder::AudioDecoder()
{
    mCodec = nullptr;
    mCodecCtx = nullptr;
    mSwrCtx = nullptr;

    wantSpec = {0};
    haveSpec = {0};

    m_stopping = false;
    processing = false;

    m_context = nullptr;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AudioDecoder::onPosChanged);
}

AudioDecoder::~AudioDecoder() {}

void AudioDecoder::onPosChanged()
{
    double position = m_context->clock.get();
    emit positionChanged(position);
}

void AudioDecoder::pushPacket()
{
    qDebugT() << "push packet from Audio Decoder";

    {
        QMutexLocker locker(&mutex);
        audioPacQueue.push(m_context->audioQueue->pop());
    }

    // trigger processing
    if ( !processing )
    {
        processing = true;
        QMetaObject::invokeMethod(this, "processQueuedPackets", Qt::AutoConnection);
    }
}

void AudioDecoder::onAudioCodecParReady(AVCodecParameters *codecpar, AVRational timeBase)
{
    qDebugT() << "AudioDecoder received codec parameters.";
    // Here you can initialize your decoder with the codecpar
    timer->start(30);

    m_timeBase = timeBase;

    mCodec = avcodec_find_decoder(codecpar->codec_id);
    mCodecCtx = avcodec_alloc_context3(mCodec);
    avcodec_parameters_to_context(mCodecCtx, codecpar);
    avcodec_open2(mCodecCtx, mCodec, nullptr);

    // ----------------------------------Init SDL----------------------------------
    // Open SDL audio device
    if ( SDL_Init(SDL_INIT_AUDIO) < 0 )
    {
        qDebugT() << "SDL_Init failed: " << SDL_GetError();
        return ;
    }
    SDL_zero(wantSpec);

    wantSpec.freq = 48000;
    // This is AV_SAMPLE_FMT_S16
    wantSpec.format = AUDIO_S16SYS;
    wantSpec.channels = 2;
    wantSpec.samples = 1024;
    wantSpec.callback = NULL;

    audioDeviceId = SDL_OpenAudioDevice(NULL, 0, &wantSpec, &haveSpec, 0);
    if( !audioDeviceId )
    {
        printf("Failed to open audio device: %s\n", SDL_GetError());
        return ;
    }

    // ---------------------------------- Init SwrContext ----------------------------------
    // Resampler setup
    AVChannelLayout in_ch_layout = mCodecCtx->ch_layout;
    AVChannelLayout out_ch_layout;
    av_channel_layout_default(&out_ch_layout, 2);

    int ret = 0;
    ret = swr_alloc_set_opts2(
        &mSwrCtx,
        &out_ch_layout, AV_SAMPLE_FMT_S16, haveSpec.freq,
        &in_ch_layout,  mCodecCtx->sample_fmt, mCodecCtx->sample_rate,
        0, NULL
    );
    if (ret < 0) {
        qDebugT() << "swr_alloc_set_opts2 failed";
        return ;
    }

    swr_init(mSwrCtx);
    // --------------------------------------------------------------------------------------

    // Start play audio
    SDL_PauseAudioDevice(audioDeviceId, 0);
}

void AudioDecoder::processQueuedPackets()
{
    // qDebugT() << "AudioDecoder::processQueuedPackets called.";

    while (true)
    {
        if (m_stopping)
        {
            break;
        }

        AVPacket* pkt = nullptr;
        {
            QMutexLocker locker(&mutex);
            if (audioPacQueue.empty())
            {
                break;
            }
            pkt = audioPacQueue.front();
            audioPacQueue.pop();
        }

        // Decode packet
        if (avcodec_send_packet(mCodecCtx, pkt) == 0) {
            AVFrame* frame = av_frame_alloc();

            while (avcodec_receive_frame(mCodecCtx, frame) == 0)
            {
                // Resample + SDL playback
                processPCM(frame);
            }

            av_frame_free(&frame);
        }

        av_packet_free(&pkt);
    }

    processing = false;
}

void AudioDecoder::stopDecode()
{
    qDebugT() << "AudioDecoder::stopDecode called.";

    m_stopping = true;

    // Clear queued packets
    {
        QMutexLocker locker(&mutex);
        while (!audioPacQueue.empty())
        {
            AVPacket* pkt = audioPacQueue.front();
            audioPacQueue.pop();
            av_packet_free(&pkt);
        }
    }

    // Close SDL audio device
    if (audioDeviceId != 0)
    {
        SDL_CloseAudioDevice(audioDeviceId);
        audioDeviceId = 0;
    }

    SDL_Quit();

    // Free SwrContext
    if (mSwrCtx)
    {
        swr_free(&mSwrCtx);
        mSwrCtx = nullptr;
    }

    // Free codec context
    if (mCodecCtx)
    {
        avcodec_free_context(&mCodecCtx);
        mCodecCtx = nullptr;
    }

    timer->stop();
}

void AudioDecoder::processPCM(AVFrame* frame)
{
    double pts = frame->pts * av_q2d(m_timeBase);

    uint8_t *out_buf = (uint8_t *)av_malloc(2 * 48000 * 2);
    // qDebugT() << "frame number: " << frame->pts;
    // av_free(out_buf);

    uint8_t *out_planes[2] = { out_buf, NULL };
    int out_samples = swr_convert(
        mSwrCtx, out_planes, frame->nb_samples,
        (const uint8_t **)frame->data, frame->nb_samples);

    int out_size = av_samples_get_buffer_size(
        NULL, haveSpec.channels, out_samples, AV_SAMPLE_FMT_S16, 1);

    while ( SDL_GetQueuedAudioSize(audioDeviceId) >  MAX_QUEUE_BYTES )
    {
        QThread::msleep(5);
    }

    SDL_QueueAudio(audioDeviceId, out_buf, out_size);

    av_free(out_buf);

    m_context->clock.set(pts);
    // qDebugT() << "Audio PTS updated to: " << pts;
}

PlayContext* AudioDecoder::getContext()
{
    return m_context;
}

void AudioDecoder::setContext(PlayContext* context)
{
    m_context = context;
}
