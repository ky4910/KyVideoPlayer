#include <QDebug>
#include <utility>
#include <memory>

#include "playercontroller.h"

PlayerController::PlayerController()
{
    videoOutputFBO = nullptr;

    m_duration = 0.0;
    m_position = 0.0;

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

    connect(dmxThread, &QThread::started, dmxWorker, &DemuxWorker::startWork);
    connect(this, &PlayerController::stopSignal, audioDecoder, &AudioDecoder::stopDecode);
    connect(this, &PlayerController::stopSignal, videoDecoder, &VideoDecoder::stopDecode);
    connect(this, &PlayerController::stopSignal, dmxWorker, &DemuxWorker::stopDemux);
    connect(dmxWorker, &DemuxWorker::audioCodecParReady, audioDecoder, &AudioDecoder::onAudioCodecParReady);
    connect(dmxWorker, &DemuxWorker::audioPacketReady, audioDecoder, &AudioDecoder::pushPacket);
    connect(dmxWorker, &DemuxWorker::videoCodecParReady, videoDecoder, &VideoDecoder::onVideoCodecParReady);
    connect(dmxWorker, &DemuxWorker::videoPacketReady, videoDecoder, &VideoDecoder::pushPacket);
    connect(dmxWorker, &DemuxWorker::durationReady, this, &PlayerController::onDurationChanged);
    connect(audioDecoder, &AudioDecoder::positionChanged, this, &PlayerController::onPositionChanged);
}

PlayerController::~PlayerController() {}

void PlayerController::play(QString url)
{
    qDebugT() << "PlayerController::play, url: " << url;

    m_ctx.reset();

    m_ctx = std::make_unique<PlayContext>();
    m_ctx->running = true;
    m_ctx->audioQueue = new PacketQueue();
    m_ctx->videoQueue = new PacketQueue();

    dmxWorker->setContext(m_ctx.get());
    audioDecoder->setContext(m_ctx.get());
    videoDecoder->setContext(m_ctx.get());

    if ( !dmxWorker->openMedia(url))
    {
        emit errorOccurred("Failed to open media: " + url);
        return;
    }

    qDebugT() << "Media opened successfully, starting threads.";

    dmxThread->start();
    videoDecThread->start();
    audioDecThread->start();
}

void PlayerController::pause()
{
    qDebugT() << "PlayerController::pause";
}

void PlayerController::seek(double sec)
{
    qDebugT() << "PlayerController::seek, sec:" << sec;
}

void PlayerController::stop()
{
    qDebugT() << "PlayerController::stop";

    emit stopSignal();

    QThread::msleep(1000);

    dmxThread->quit();
    dmxThread->wait();

    audioDecThread->quit();
    audioDecThread->wait();

    videoDecThread->quit();
    qDebugT() << "Waiting for VIDEO to finish...";
    videoDecThread->wait();
    qDebugT() << "VIDEO finished.";
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
