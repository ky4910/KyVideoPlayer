#include <QDebug>
#include <utility>
#include <memory>

#include "playercontroller.h"

PlayerController::PlayerController()
{
    videoOutputFBO = nullptr;

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
}

PlayerController::~PlayerController() {}

void PlayerController::play(QString url)
{
    qDebugT() << "PlayerController::play, url: " << url;

    m_ctx = std::make_unique<PlayContext>();
    m_ctx->running = true;

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

    audioDecThread->quit();
    audioDecThread->wait();

    videoDecThread->quit();
    qDebugT() << "Waiting for VIDEO to finish...";
    videoDecThread->wait();
    qDebugT() << "VIDEO finished.";

    dmxThread->quit();
    dmxThread->wait();
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

