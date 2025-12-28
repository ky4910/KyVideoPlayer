#ifndef AUDIODECODER_H
#define AUDIODECODER_H

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

class AudioDecoder : public QObject
{
    Q_OBJECT

public:
    AudioDecoder();
    virtual ~AudioDecoder();

private:
    void processPCM(AVFrame* frame);

public slots:
    void pushPacket(AVPacket *pkt);
    void onAudioCodecParReady(AVCodecParameters *codecpar, AVRational timeBase);
    void processQueuedPackets();
    void stopDecode();

    PlayContext* getContext();
    void setContext(PlayContext* context);

private:
    QMutex mutex;
    const AVCodec   *mCodec;
    AVCodecContext  *mCodecCtx;
    SwrContext      *mSwrCtx;
    SDL_AudioSpec   wantSpec, haveSpec;
    SDL_AudioDeviceID audioDeviceId;

    bool running;
    bool processing;
    std::queue<AVPacket*> audioPacQueue;

    PlayContext* m_context;
    AVRational m_timeBase;
};

#endif // AUDIODECODER_H
