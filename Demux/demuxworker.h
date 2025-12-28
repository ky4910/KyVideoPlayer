#ifndef DEMUXWORKER_H
#define DEMUXWORKER_H

#include <QObject>
#include <libavcodec/packet.h>
#include <libavcodec/codec_par.h>
#include <libavformat/avformat.h>

#include "utils.h"
#include "playcontext.h"

class DemuxWorker : public QObject
{
    Q_OBJECT

public:
    DemuxWorker();
    virtual ~DemuxWorker();

    void startWork();
    void stopWork();

    bool openMedia(QString path);

    PlayContext* getContext();
    void setContext(PlayContext* context);

signals:

    void audioPacketReady();
    void videoPacketReady();

    void audioCodecParReady(AVCodecParameters *codecpar, AVRational timeBase);
    void videoCodecParReady(AVCodecParameters *codecpar, AVRational timeBase);

    void durationReady(double duration);

public slots:
    void stopDemux();

private:
    int audioIndex;
    int videoIndex;
    bool running;
    AVFormatContext *fmtCtx;
    PlayContext* m_context;
};

#endif // DEMUXWORKER_H
