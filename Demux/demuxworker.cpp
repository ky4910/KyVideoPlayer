#include <QTime>
#include <QDebug>
#include <QThread>

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libavcodec/avcodec.h>
}

#include "demuxworker.h"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

DemuxWorker::DemuxWorker()
{
    fmtCtx = nullptr;
    audioIndex = -1;
    videoIndex = -1;

    SDL_version compiled;
    SDL_VERSION(&compiled);

    qDebugT() << "Compiled SDL version: "
             << static_cast<int>(compiled.major) << "."
             << static_cast<int>(compiled.minor) << "."
             << static_cast<int>(compiled.patch);
}

DemuxWorker::~DemuxWorker() {}

void DemuxWorker::startWork()
{
    qDebugT() << "DemuxWorker::startWork called.";
    // Placeholder for actual demuxing work
    if (!fmtCtx)
        return;

    while ( m_context->running )
    {
        AVPacket* pkt = av_packet_alloc();
        if ( m_context->paused )
        {
            QThread::msleep(10);
            continue;
        }

        int ret = av_read_frame(fmtCtx, pkt);
        if (ret < 0)
        {
            av_packet_free(&pkt);
            break;
        }

        if (pkt->stream_index == audioIndex)
        {
            m_context->audioQueue->push(av_packet_clone(pkt));
            // emit audioPacketReady();
        }
        else if (pkt->stream_index == videoIndex)
        {
            m_context->videoQueue->push(av_packet_clone(pkt));
            // emit videoPacketReady();
        }
        else
        {
            av_packet_free(&pkt);
        }
    }

    qDebugT() << "dexmuxing thread exiting.";
}

void DemuxWorker::stopWork()
{
    // Placeholder for stopping demuxing work
}

void DemuxWorker::openMedia(QString path)
{
    QString errorMsg;

    qDebugT() << "DemuxWorker::openMedia called with path:" << path;
    // Placeholder for opening media file
    int st_index[AVMEDIA_TYPE_NB];

    fmtCtx = avformat_alloc_context();
    if ( !fmtCtx )
    {
        qDebugT() << "Could not allocate AVFormatContext.";
        errorMsg = "Could not allocate AVFormatContext.";
        emit openResult(false, errorMsg);
    }

    char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
    QString trimmedPath = path.trimmed();
    if ( avformat_open_input(&fmtCtx, trimmedPath.toStdString().c_str(), nullptr, nullptr) != 0 )
    {
        qDebugT() << "Error opening input: " << av_make_error_string(errbuf, AV_ERROR_MAX_STRING_SIZE, AVERROR(errno));
        errorMsg = QString("Error opening input: ") + av_make_error_string(errbuf, AV_ERROR_MAX_STRING_SIZE, AVERROR(errno));;
        emit openResult(false, errorMsg);
        avformat_free_context(fmtCtx);
    }

    if ( avformat_find_stream_info(fmtCtx, nullptr) < 0 )
    {
        qDebugT() << "Could not find stream information.";
        errorMsg = "Could not find stream information.";
        emit openResult(false, errorMsg);
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
    }

    // ------------------- AUDIO Codec Param Find -------------------

    AVCodecParameters *audioCodecparCopy = nullptr;

    audioIndex = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (audioIndex < 0)
    {
        qDebugT() << "Could not find audio stream.";
        errorMsg = "Could not find audio stream.";
        emit openResult(false, errorMsg);
    }
    else
    {
        AVStream *audio_stream = fmtCtx->streams[audioIndex];
        audioCodecparCopy = avcodec_parameters_alloc();
        if (!audioCodecparCopy)
        {
            qDebugT() << "Could not allocate AVCodecParameters for audio.";
            errorMsg = "Could not allocate AVCodecParameters for audio.";
            emit openResult(false, errorMsg);
        }
        else
        {
            if (avcodec_parameters_copy(audioCodecparCopy, audio_stream->codecpar) < 0)
            {
                qDebugT() << "Could not copy audio codec parameters.";
                errorMsg = "Could not copy audio codec parameters.";
                emit openResult(false, errorMsg);
                avcodec_parameters_free(&audioCodecparCopy);
                audioCodecparCopy = nullptr;
            }
            else
            {
                emit audioCodecParReady(audioCodecparCopy, audio_stream->time_base);
            }
        }
    }

    // ------------------- VIDEO Codec Param Find -------------------
    AVCodecParameters *videoCodecparCopy = nullptr;

    videoIndex = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (videoIndex < 0)
    {
        qDebugT() << "Could not find video stream.";
        errorMsg = "Could not find video stream.";
        emit openResult(false, errorMsg);
    }
    else
    {
        AVStream *video_stream = fmtCtx->streams[videoIndex];
        videoCodecparCopy = avcodec_parameters_alloc();
        if (!videoCodecparCopy)
        {
            qDebugT() << "Could not allocate AVCodecParameters for video.";
            errorMsg = "Could not allocate AVCodecParameters for video.";
            emit openResult(false, errorMsg);
        }
        else
        {
            if (avcodec_parameters_copy(videoCodecparCopy, video_stream->codecpar) < 0)
            {
                qDebugT() << "Could not copy video codec parameters.";
                errorMsg = "Could not copy video codec parameters.";
                emit openResult(false, errorMsg);
                avcodec_parameters_free(&videoCodecparCopy);
                videoCodecparCopy = nullptr;
            }
            else
            {
                emit videoCodecParReady(videoCodecparCopy, video_stream->time_base);
            }
        }
    }

    qDebugT() << "Media file opened successfully:" << path;

    if (audioIndex < 0 && videoIndex < 0)
    {
        qDebugT() << "No audio or video streams found.";
        errorMsg = "No audio or video streams found.";
        emit openResult(false, errorMsg);
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
    }

    double duration = fmtCtx->duration / (double)AV_TIME_BASE;
    qDebugT() << "Media duration: " << duration << " seconds.";

    emit durationReady(duration);
    errorMsg = "Open Success";
    emit openResult(true, errorMsg);
}

void DemuxWorker::stopDemux()
{
    qDebugT() << "DemuxWorker::stopDemux called.";

    m_context->audioQueue->setRunning(false);
    m_context->videoQueue->setRunning(false);

    if ( fmtCtx )
    {
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
        fmtCtx = nullptr;
    }
}

void DemuxWorker::pauseDemux()
{
    qDebugT() << "DemuxWorker::pauseDemux called.";
    // Implement pause functionality if needed
}

void DemuxWorker::resumeDemux()
{
    qDebugT() << "DemuxWorker::resumeDemux called.";
    // Implement resume functionality if needed
    m_context->paused = false;
}

PlayContext* DemuxWorker::getContext()
{
    return m_context;
}

void DemuxWorker::setContext(PlayContext* context)
{
    m_context = context;
}
