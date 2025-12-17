#include <QDebug>

#include "playercontroller.h"

PlayerController::PlayerController()
{
    dmxThread = nullptr;
    audioDecThread = nullptr;
    videoDecThread = nullptr;

    dmxWorker = nullptr;
    audioDecoder = nullptr;
    // videoDecoder = nullptr;
    videoOutputFBO = nullptr;

    videoDecoder = new VideoDecoder();
}

PlayerController::~PlayerController() {}

void PlayerController::play(QString url)
{
    // stop();

    dmxThread = new QThread;
    audioDecThread = new QThread;
    videoDecThread = new QThread;

    this->setObjectName("PlayerController");
    dmxThread->setObjectName("DemuxThread");
    audioDecThread->setObjectName("AudioDecodeThread");
    videoDecThread->setObjectName("VideoDecodeThread");

    dmxWorker = new DemuxWorker(url);
    audioDecoder = new AudioDecoder();

    dmxWorker->moveToThread(dmxThread);
    audioDecoder->moveToThread(audioDecThread);
    videoDecoder->moveToThread(videoDecThread);

    qDebugT() << "PlayerController::play, url: " << url;

    connect(dmxThread, &QThread::started, dmxWorker, &DemuxWorker::startWork);
    connect(this, &PlayerController::stopSignal, audioDecoder, &AudioDecoder::stopDecode);
    connect(this, &PlayerController::stopSignal, videoDecoder, &VideoDecoder::stopDecode);
    connect(dmxWorker, &DemuxWorker::audioCodecParReady, audioDecoder, &AudioDecoder::onAudioCodecParReady);
    connect(dmxWorker, &DemuxWorker::audioPacketReady, audioDecoder, &AudioDecoder::pushPacket);
    connect(dmxWorker, &DemuxWorker::videoCodecParReady, videoDecoder, &VideoDecoder::onVideoCodecParReady);
    connect(dmxWorker, &DemuxWorker::videoPacketReady, videoDecoder, &VideoDecoder::pushPacket);

    if ( !dmxWorker->openMedia(url))
    {
        emit errorOccurred("Failed to open media: " + url);
        return;
    }

    qDebugT() << "Media opened successfully, starting threads.";

    audioDecThread->start();
    videoDecThread->start();
    dmxThread->start();
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

    dmxThread->quit();
    dmxThread->wait();

    audioDecThread->quit();
    audioDecThread->wait();
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

