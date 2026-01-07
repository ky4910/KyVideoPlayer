#include <QDebug>
#include <utility>
#include <memory>

#include "playercontroller.h"

PlayerController::PlayerController()
{
    videoOutputFBO = nullptr;
    m_state = PlayState::Stopped;

    m_duration = 0.0;
    m_position = 0.0;

    audioDone = false;
    videoDone = false;

    m_timer = new QTimer(this);

    dmxThread = new QThread;
    audioDecThread = new QThread;
    videoDecThread = new QThread;

    this->setObjectName("PlayerController");
    dmxThread->setObjectName("DemuxThread");
    audioDecThread->setObjectName("AudioDecodeThread");
    videoDecThread->setObjectName("VideoDecodeThread");

    dmxWorker = new DemuxWorker();
    audioDecoder = new AudioDecoder();
    videoDecoder = new VideoDecoder();

    dmxWorker->moveToThread(dmxThread);
    audioDecoder->moveToThread(audioDecThread);
    videoDecoder->moveToThread(videoDecThread);

    // connect(dmxThread, &QThread::started, dmxWorker, &DemuxWorker::startWork);

    // connect(this, &PlayerController::stopSignal, dmxWorker, &DemuxWorker::stopDemux);
    // connect(this, &PlayerController::stopSignal, audioDecoder, &AudioDecoder::stopDecode);
    // connect(this, &PlayerController::stopSignal, videoDecoder, &VideoDecoder::stopDecode);
    connect(this, &PlayerController::openFile, dmxWorker, &DemuxWorker::openMedia);
    connect(dmxWorker, &DemuxWorker::openResult, this, &PlayerController::onOpenResult);
    connect(this, &PlayerController::pauseSignal, dmxWorker, &DemuxWorker::pauseDemux);
    connect(this, &PlayerController::pauseSignal, audioDecoder, &AudioDecoder::onPauseAudio);
    connect(this, &PlayerController::pauseSignal, videoDecoder, &VideoDecoder::onPauseVideo);
    connect(this, &PlayerController::resumeSignal, dmxWorker, &DemuxWorker::resumeDemux);
    connect(this, &PlayerController::resumeSignal, audioDecoder, &AudioDecoder::onResumeAudio);
    connect(this, &PlayerController::resumeSignal, videoDecoder, &VideoDecoder::onResumeVideo);

    connect(this, &PlayerController::startDemux, dmxWorker, &DemuxWorker::startWork);

    connect(dmxWorker, &DemuxWorker::audioCodecParReady, audioDecoder, &AudioDecoder::onAudioCodecParReady);
    connect(dmxWorker, &DemuxWorker::videoCodecParReady, videoDecoder, &VideoDecoder::onVideoCodecParReady);

    connect(this, &PlayerController::startDecode, audioDecoder, &AudioDecoder::doDecode);
    connect(this, &PlayerController::startDecode, videoDecoder, &VideoDecoder::doDecode);

    connect(dmxWorker, &DemuxWorker::durationReady, this, &PlayerController::onDurationChanged);
    // connect(audioDecoder, &AudioDecoder::positionChanged, this, &PlayerController::onPositionChanged);
    connect(m_timer, &QTimer::timeout, this, &PlayerController::onUpdateProgress);

    connect(audioDecoder, &AudioDecoder::decodeFinshed, this, &PlayerController::onAudioFinished);
    connect(videoDecoder, &VideoDecoder::decodeFinshed, this, &PlayerController::onVideoFinished);
}

PlayerController::~PlayerController() {}

void PlayerController::play(QString url)
{
    qDebugT() << "PlayerController::play, url: " << url;

    m_ctx.reset();

    videoOutputFBO->setPlayState(PlayState::Playing);

    m_ctx = std::make_unique<PlayContext>();
    m_ctx->running = true;
    m_ctx->paused = false;
    m_ctx->audioQueue = new PacketQueue();
    m_ctx->videoQueue = new PacketQueue();

    dmxWorker->setContext(m_ctx.get());
    audioDecoder->setContext(m_ctx.get());
    videoDecoder->setContext(m_ctx.get());

    videoDecThread->start();
    audioDecThread->start();
    dmxThread->start();

    // emit the singal to demux worker to open media
    emit openFile(url);

    qDebugT() << "Media opened successfully, starting threads.";

    m_state = PlayState::Playing;
    emit playStateChanged();

    m_timer->start(500);
}

void PlayerController::onOpenResult(bool success, const QString& errorMsg)
{
    if ( !success )
    {
        qDebugT() << "PlayerController::onOpenResult failed: " << errorMsg;
        // TO-DO stop handler
    }
    else
    {
        qDebugT() << "PlayerController::onOpenResult succeeded.";
        emit startDemux();

        QThread::msleep(100);
        emit startDecode();
    }
}

void PlayerController::onUpdateProgress()
{
    if ( m_ctx && m_ctx->running )
    {
        double pos = m_ctx->clock.get();
        onPositionChanged(pos);
    }
}

void PlayerController::pause()
{
    qDebugT() << "PlayerController::pause";

    m_state = PlayState::Paused;
    emit playStateChanged();
    m_ctx->paused = true;

    emit pauseSignal();
}

void PlayerController::resume()
{
    qDebugT() << "PlayerController::resume";

    m_state = PlayState::Playing;
    emit playStateChanged();
    m_ctx->paused = false;

    emit resumeSignal();
}

void PlayerController::seek(double sec)
{
    qDebugT() << "PlayerController::seek, sec:" << sec;
}

void PlayerController::stop()
{
    qDebugT() << "PlayerController::stop";

    m_ctx->running = false;
    m_ctx->stopped = true;
    m_ctx->audioQueue->abort();
    m_ctx->videoQueue->abort();

    // disconnect(videoDecoder, nullptr, videoOutputFBO, nullptr);

    videoOutputFBO->setPlayState(PlayState::Stopped);
    videoOutputFBO->clearScreen();
    qDebugT() << "clear the screen!";

    emit stopSignal();

    dmxThread->quit();
    audioDecThread->quit();
    videoDecThread->quit();

    qDebugT() << "quit 3 threads!";

    dmxThread->wait();
    qDebugT() << "dmxThread wait finieshed!!!";
    audioDecThread->wait();
    qDebugT() << "audioDecThread wait finieshed!!!";
    videoDecThread->wait();

    qDebugT() << "wait finieshed!!!";

    m_ctx->audioQueue->clear();
    m_ctx->videoQueue->clear();

    m_ctx.reset();
    m_state = PlayState::Stopped;
    emit playStateChanged();

    onPositionChanged(0.0);

    qDebugT() << "stop operation done!";
}

void PlayerController::setVideoOutput(QObject *videoItem)
{
    auto output = qobject_cast<VideoOutputFBO *>(videoItem);
    if (!output) {
        qWarning() << "setVideoOutput: invalid item";
        return;
    }

    videoOutputFBO = output;

    connect(videoDecoder, &VideoDecoder::frameDecoded,
            videoOutputFBO,  &VideoOutputFBO::processYUV,
            Qt::QueuedConnection);
}

void PlayerController::onDurationChanged(double duration)
{
    if (qFuzzyCompare(m_duration, duration))
        return;

    m_duration = duration;

    emit durationChanged();
}

void PlayerController::onPositionChanged(double position)
{
    if (qFuzzyCompare(m_position, position))
        return;

    m_position = position;
    emit positionChanged();
}

void PlayerController::onAudioFinished()
{
    qDebugT() << "AudioDecoder signaled decode finished.";
    audioDone = true;
    if ( videoDone )
    {
        qDebugT() << "AUDIO: Both audio and video finished.";
        playFinished();
    }
}

void PlayerController::onVideoFinished()
{
    qDebugT() << "VideoDecoder signaled decode finished.";
    videoDone = true;
    if ( audioDone )
    {
        qDebugT() << "VIDEO: Both audio and video finished.";
        playFinished();
    }
}

void PlayerController::playFinished()
{
    stop();

    qDebugT() << "Playback finished.";

    videoOutputFBO->clearScreen();

    m_state = PlayState::Finished;
    emit playStateChanged();
}
