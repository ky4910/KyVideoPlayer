#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include <queue>
#include <QObject>
#include <QMutex>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "playcontext.h"

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
    void syncAndSend(AVFrame* frame, double diff);

signals:
    void frameDecoded(AVFrame* frame);
    void decodeFinshed();

public slots:
    void onVideoCodecParReady(AVCodecParameters *codecpar, AVRational timeBase);
    void doDecode();
    void stopDecode();
    void onPauseVideo();
    void onResumeVideo();

    PlayContext* getContext();
    void setContext(PlayContext* context);

private:
    QMutex mutex;
    const AVCodec   *mCodec;
    AVCodecContext  *mCodecCtx;
    SwrContext      *mSwrCtx;

    std::queue<AVPacket*> videoPacQueue;

    PlayContext* m_context;
    AVRational m_timeBase;
};

#endif // VIDEODECODER_H

