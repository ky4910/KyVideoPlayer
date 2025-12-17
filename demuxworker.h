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
    DemuxWorker(QString path);
    virtual ~DemuxWorker();

    void startWork();
    void stopWork();

    bool openMedia(QString path);

signals:
    void finished();

    void audioPacketReady(AVPacket *pkt);
    void videoPacketReady(AVPacket *pkt);

    void audioCodecParReady(AVCodecParameters *codecpar);
    void videoCodecParReady(AVCodecParameters *codecpar);

private:
    int audioIndex;
    int videoIndex;
    bool running;
    AVFormatContext *fmtCtx;
    // std::atomic<bool> m_running { false };
    // AVFormatContext *fmtCtx = nullptr;
};

#endif // DEMUXWORKER_H
