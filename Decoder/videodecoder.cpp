#include "videodecoder.h"
#include "utils.h"

#include <QThread>

constexpr double SYNC_THRESHOLD = 0.03;  // 30 ms
constexpr double MAX_DELAY      = 0.1;   // 100 ms

VideoDecoder::VideoDecoder(QObject *parent)
    : QObject{parent}
{
    mCodec = nullptr;
    mCodecCtx = nullptr;
    mSwrCtx = nullptr;

    m_stopping = false;
    processing = false;

    m_context = nullptr;
}

VideoDecoder::~VideoDecoder() {}

void VideoDecoder::onVideoCodecParReady(AVCodecParameters *codecpar, AVRational timeBase)
{
    qDebugT() << "VideoDecoder received codec parameters.";
    // Here you can initialize your decoder with the codecpar

    m_timeBase = timeBase;

    mCodec = avcodec_find_decoder(codecpar->codec_id);
    mCodecCtx = avcodec_alloc_context3(mCodec);
    avcodec_parameters_to_context(mCodecCtx, codecpar);
    avcodec_open2(mCodecCtx, mCodec, nullptr);
}

void VideoDecoder::doDecode()
{
    qDebugT() << "VideoDecoder::doDecode called.";

    while (m_context->running)
    {
        if ( m_context->paused )
        {
            QThread::msleep(10);
            continue;
        }

        if ( !m_context->running )
        {
            processing = false;
            break;
        }

        AVPacket* pkt = nullptr;
        {
            QMutexLocker locker(&mutex);
            if ( m_context->videoQueue->empty() )
            {
                break;
            }
            pkt = m_context->videoQueue->pop();
        }

        // Decode packet
        if (avcodec_send_packet(mCodecCtx, pkt) == 0) {
            AVFrame* frame = av_frame_alloc();

            while (avcodec_receive_frame(mCodecCtx, frame) == 0)
            {
                // Send video data to OpenGL or QQuickFramebufferObject
                AVFrame *out = av_frame_alloc();
                av_frame_ref(out, frame);

                int64_t pts = (frame->pts != AV_NOPTS_VALUE)
                                ? frame->pts
                                : frame->best_effort_timestamp;

                double videoPts = pts * av_q2d(m_timeBase);
                double audioPts = m_context->clock.get();
                double diff = videoPts - audioPts;
                // qDebugT() << "Video PTS: " << videoPts << ", Audio PTS: " << audioPts;
                // QThread::msleep(30);
                syncAndSend(out, diff);
                // emit frameDecoded(out);
            }

            av_frame_free(&frame);
        }

        av_packet_free(&pkt);
    }

    if ( m_context->stopped )
    {
        m_context->videoQueue->clear();
    }

    processing = false;
}

void VideoDecoder::processQueuedPackets()
{
    // qDebugT() << "VideoDecoder::processQueuedPackets called.";

    // while ( true )
    while (m_context->running)
    {
        if ( m_context->paused )
        {
            QThread::msleep(10);
            continue;
        }

        if ( !m_context->running )
        {
            processing = false;
            break;
        }

        // if ( m_stopping )
        // {
        //     break;
        // }

        AVPacket* pkt = nullptr;
        {
            QMutexLocker locker(&mutex);
            if (videoPacQueue.empty())
            {
                break;
            }
            pkt = videoPacQueue.front();
            videoPacQueue.pop();
        }

        // Decode packet
        if (avcodec_send_packet(mCodecCtx, pkt) == 0) {
            AVFrame* frame = av_frame_alloc();

            while (avcodec_receive_frame(mCodecCtx, frame) == 0)
            {
                // Send video data to OpenGL or QQuickFramebufferObject
                AVFrame *out = av_frame_alloc();
                av_frame_ref(out, frame);

                int64_t pts = (frame->pts != AV_NOPTS_VALUE)
                                ? frame->pts
                                : frame->best_effort_timestamp;

                double videoPts = pts * av_q2d(m_timeBase);
                double audioPts = m_context->clock.get();
                double diff = videoPts - audioPts;
                // qDebugT() << "Video PTS: " << videoPts << ", Audio PTS: " << audioPts;
                // QThread::msleep(30);
                syncAndSend(out, diff);
                // emit frameDecoded(out);
            }

            av_frame_free(&frame);
        }

        av_packet_free(&pkt);
    }

    if ( m_context->stopped )
    {
        m_context->videoQueue->clear();
    }

    processing = false;

    // qDebugT() << "VideoDecoder::processQueuedPackets finished.";
}

void VideoDecoder::stopDecode()
{
    //
    qDebugT() << "VideoDecoder::stopDecode called.";

    m_stopping = true;

    // Clear queued packets
    {
        QMutexLocker locker(&mutex);
        while (!videoPacQueue.empty())
        {
            AVPacket* pkt = videoPacQueue.front();
            videoPacQueue.pop();
            av_packet_free(&pkt);
        }
    }

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

    // AVFrame *nullFrame = nullptr;
    // emit frameDecoded(nullFrame);

    qDebugT() << "VideoDecoder resources freed.";
}

void VideoDecoder::onPauseVideo()
{
    qDebugT() << "VideoDecoder::onPauseVideo called.";
    // Handle pause if needed
}

void VideoDecoder::onResumeVideo()
{
    qDebugT() << "VideoDecoder::onResumeVideo called.";
    // Handle resume if needed
}

void VideoDecoder::syncAndSend(AVFrame* frame, double diff)
{
    if (diff > SYNC_THRESHOLD)
    {
        // video ahead → delay frame display
        QThread::usleep(std::min(diff, MAX_DELAY) * 1e6);
    }
    else if (diff < -MAX_DELAY)
    {
        qDebugT() << "VideoDecoder dropping frame, diff: " << diff;
        if ( !frame->key_frame )
        {
            // video behind → drop frame
            av_frame_free(&frame);
            return;
        }
    }
    emit frameDecoded(frame);
}

PlayContext* VideoDecoder::getContext()
{
    return m_context;
}

void VideoDecoder::setContext(PlayContext* context)
{
    m_context = context;
}
