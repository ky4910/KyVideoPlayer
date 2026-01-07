#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QTimer>

#include "utils.h"
#include "demuxworker.h"
#include "audiodecoder.h"
#include "videodecoder.h"
#include "videooutputfbo.h"
#include "playcontext.h"
#include "playstate.h"

class PlayerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PlayState playState READ playState NOTIFY playStateChanged)
    Q_PROPERTY(double duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(double position READ position NOTIFY positionChanged)

public:
    PlayerController();
    ~PlayerController();

    Q_INVOKABLE void play(QString url);
    Q_INVOKABLE void seek(double sec);
    Q_INVOKABLE void stop();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void resume();
    Q_INVOKABLE void setVideoOutput(QObject *videoItem);

    double duration() const { return m_duration; }
    double position() const { return m_position; }
    PlayState playState() const { return m_state; }

    void playFinished();

signals:
    void openFile(const QString &path);
    void startDemux();
    void startDecode();
    void stopSignal();
    void pauseSignal();
    void resumeSignal();

    void durationChanged();
    void positionChanged();
    void playStateChanged();
    void errorOccurred(const QString &message);

public slots:
    void onOpenResult(bool success, const QString& errorMsg);
    void onDurationChanged(double duration);
    void onPositionChanged(double position);
    void onUpdateProgress();
    void onAudioFinished();
    void onVideoFinished();

private:
    QThread *dmxThread;
    QThread *audioDecThread;
    QThread *videoDecThread;

    DemuxWorker *dmxWorker;
    AudioDecoder *audioDecoder;
    VideoDecoder *videoDecoder;
    VideoOutputFBO *videoOutputFBO;

    double m_duration;
    double m_position;

    bool audioDone;
    bool videoDone;

    std::unique_ptr<PlayContext> m_ctx;

    PlayState m_state;
    QTimer *m_timer;
};

#endif // PLAYERCONTROLLER_H
