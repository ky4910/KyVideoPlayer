#ifndef DEMUXWORKER_H
#define DEMUXWORKER_H

#include <QObject>
#include <libavcodec/packet.h>
#include <libavcodec/codec_par.h>
#include <libavformat/avformat.h>

#include "utils.h"

class DemuxWorker : public QObject
{
    Q_OBJECT

public:
    DemuxWorker();
    virtual ~DemuxWorker();

    void startWork();
    void stopWork();

    bool openMedia(QString path);

signals:

    void audioPacketReady(AVPacket *pkt);
    void videoPacketReady(AVPacket *pkt);

    void audioCodecParReady(AVCodecParameters *codecpar, AVRational timeBase);
    void videoCodecParReady(AVCodecParameters *codecpar, AVRational timeBase);

public slots:
    void stopDemux();

private:
    int audioIndex;
    int videoIndex;
    bool running;
    AVFormatContext *fmtCtx;
};

#endif // DEMUXWORKER_H
