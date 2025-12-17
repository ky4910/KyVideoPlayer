#include "videodecoder.h"
#include "utils.h"

#include <QThread>

VideoDecoder::VideoDecoder(QObject *parent)
    : QObject{parent}
{
    mCodec = nullptr;
    mCodecCtx = nullptr;
    mSwrCtx = nullptr;

    running = false;
    processing = false;
}

VideoDecoder::~VideoDecoder() {}

void VideoDecoder::pushPacket(AVPacket *pkt)
{
    // qDebugT() << "push packet from Video Decoder";

    {
        QMutexLocker locker(&mutex);
        videoPacQueue.push(pkt);
    }

    // trigger processing
    if ( !processing )
    {
        processing = true;
        QMetaObject::invokeMethod(this, "processQueuedPackets", Qt::AutoConnection);
    }
}

void VideoDecoder::onVideoCodecParReady(AVCodecParameters *codecpar)
{
    qDebugT() << "VideoDecoder received codec parameters.";
    // Here you can initialize your decoder with the codecpar

    mCodec = avcodec_find_decoder(codecpar->codec_id);
    mCodecCtx = avcodec_alloc_context3(mCodec);
    avcodec_parameters_to_context(mCodecCtx, codecpar);
    avcodec_open2(mCodecCtx, mCodec, nullptr);
}

void VideoDecoder::processQueuedPackets()
{
    // qDebugT() << "VideoDecoder::processQueuedPackets called.";

    while (true)
    {
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
                QThread::msleep(30);
                emit frameDecoded(out);
            }

            av_frame_free(&frame);
        }

        av_packet_free(&pkt);
    }

    processing = false;
}

void VideoDecoder::stopDecode()
{
    //
    qDebugT() << "VideoDecoder::stopDecode called.";

    running = false;

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
}


