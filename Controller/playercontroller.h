#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <QObject>
#include <QThread>

#include "utils.h"
#include "demuxworker.h"
#include "audiodecoder.h"
#include "videodecoder.h"
#include "videooutputfbo.h"
#include "playcontext.h"

class PlayerController : public QObject
{
    Q_OBJECT

public:
    PlayerController();
    ~PlayerController();

    Q_INVOKABLE void play(QString url);
    Q_INVOKABLE void pause();
    Q_INVOKABLE void seek(double sec);
    Q_INVOKABLE void stop();
    Q_INVOKABLE void setVideoOutput(QObject *videoItem);

signals:
    void stopSignal();
    void errorOccurred(const QString &message);

private:
    QThread *dmxThread;
    QThread *audioDecThread;
    QThread *videoDecThread;

    DemuxWorker *dmxWorker;
    AudioDecoder *audioDecoder;
    VideoDecoder *videoDecoder;
    VideoOutputFBO *videoOutputFBO;

    std::unique_ptr<PlayContext> m_ctx;
};

#endif // PLAYERCONTROLLER_H
