#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include <queue>
#include <QObject>
#include <QMutex>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

extern "C"
{
    #include <libavcodec/packet.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libswresample/swresample.h>
}

class VideoDecoder : public QObject
{
    Q_OBJECT

public:
    explicit VideoDecoder(QObject *parent = nullptr);
    virtual ~VideoDecoder();

signals:
    void frameDecoded(AVFrame* frame);

public slots:
    void pushPacket(AVPacket *pkt);
    void onVideoCodecParReady(AVCodecParameters *codecpar);
    void processQueuedPackets();
    void stopDecode();

private:
    QMutex mutex;
    const AVCodec   *mCodec;
    AVCodecContext  *mCodecCtx;
    SwrContext      *mSwrCtx;

    bool running;
    bool processing;
    std::queue<AVPacket*> videoPacQueue;
};

#endif // VIDEODECODER_H

