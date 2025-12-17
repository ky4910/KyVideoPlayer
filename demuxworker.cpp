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

DemuxWorker::DemuxWorker() {}

DemuxWorker::DemuxWorker(QString path)
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
    // Placeholder for actual demuxing work
    if (!fmtCtx)
        return;

    running = true;
    AVPacket* pkt = av_packet_alloc();

    while (running)
    {
        int ret = av_read_frame(fmtCtx, pkt);
        if (ret < 0)
            break;

        if (pkt->stream_index == audioIndex)
        {
            emit audioPacketReady(av_packet_clone(pkt));
        }
        else if (pkt->stream_index == videoIndex)
        {
            emit videoPacketReady(av_packet_clone(pkt));
        }

        av_packet_unref(pkt);
    }
}

void DemuxWorker::stopWork()
{
    // Placeholder for stopping demuxing work
}

bool DemuxWorker::openMedia(QString path)
{
    // Placeholder for opening media file
    int st_index[AVMEDIA_TYPE_NB];

    fmtCtx = avformat_alloc_context();
    if ( !fmtCtx )
    {
        qDebugT() << "Could not allocate AVFormatContext.";
        return false;
    }

    char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
    QString trimmedPath = path.trimmed();
    if ( avformat_open_input(&fmtCtx, trimmedPath.toStdString().c_str(), nullptr, nullptr) != 0 )
    {
        qDebugT() << "Error opening input: " << av_make_error_string(errbuf, AV_ERROR_MAX_STRING_SIZE, AVERROR(errno));
        avformat_free_context(fmtCtx);
        return false;
    }

    if ( avformat_find_stream_info(fmtCtx, nullptr) < 0 )
    {
        qDebugT() << "Could not find stream information.";
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
        return false;
    }

    // ------------------- AUDIO Codec Param Find -------------------

    audioIndex = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (audioIndex < 0)
    {
        qDebugT() << "Could not find audio stream.";
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
        return false;
    }

    AVStream *audio_stream = fmtCtx->streams[audioIndex];
    // AVCodecParameters *audioCodecpar = audio_stream->codecpar;
    AVCodecParameters *audioCodecparCopy = avcodec_parameters_alloc();
    if (!audioCodecparCopy)
    {
        qDebugT() << "Could not allocate AVCodecParameters.";
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
        return false;
    }

    if (avcodec_parameters_copy(audioCodecparCopy, audio_stream->codecpar) < 0)
    {
        qDebugT() << "Could not copy codec parameters.";
        avcodec_parameters_free(&audioCodecparCopy);
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
        return false;
    }

    emit audioCodecParReady(audioCodecparCopy);

    // ------------------- VIDEO Codec Param Find -------------------
    videoIndex = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (videoIndex < 0)
    {
        qDebugT() << "Could not find video stream.";
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
        return false;
    }

    AVStream *video_stream = fmtCtx->streams[videoIndex];
    // AVCodecParameters *videoCodecpar = video_stream->codecpar;
    AVCodecParameters *videoCodecparCopy = avcodec_parameters_alloc();
    if (!videoCodecparCopy)
    {
        qDebugT() << "Could not allocate AVCodecParameters.";
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
        return false;
    }

    if (avcodec_parameters_copy(videoCodecparCopy, video_stream->codecpar) < 0)
    {
        qDebugT() << "Could not copy codec parameters.";
        avcodec_parameters_free(&videoCodecparCopy);
        avformat_close_input(&fmtCtx);
        avformat_free_context(fmtCtx);
        return false;
    }

    emit videoCodecParReady(videoCodecparCopy);

    qDebugT() << "Media file opened successfully:" << path;

    return true;
}


