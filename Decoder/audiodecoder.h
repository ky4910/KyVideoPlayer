#ifndef AUDIODECODER_H
#define AUDIODECODER_H

#include <queue>
#include <QObject>
#include <QMutex>
#include <QTimer>

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
    void onAudioCodecParReady(AVCodecParameters *codecpar, AVRational timeBase);
    void doDecode();
    void processQueuedPackets();
    void stopDecode();
    void onPauseAudio();
    void onResumeAudio();

    PlayContext* getContext();
    void setContext(PlayContext* context);

private:
    QMutex mutex;
    const AVCodec   *mCodec;
    AVCodecContext  *mCodecCtx;
    SwrContext      *mSwrCtx;
    SDL_AudioSpec   wantSpec, haveSpec;
    SDL_AudioDeviceID audioDeviceId;

    bool m_stopping;
    bool processing;
    std::queue<AVPacket*> audioPacQueue;

    PlayContext* m_context;
    AVRational m_timeBase;
};

#endif // AUDIODECODER_H
