#ifndef PACKETQUEUE_H
#define PACKETQUEUE_H

#include <queue>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>

extern "C"
{
    #include <libavcodec/packet.h>
    #include <libavcodec/avcodec.h>
}

#define MAX_QUEUE_SIZE 5

class PacketQueue
{
public:
    PacketQueue();
    void push(AVPacket* pkt);
    AVPacket* pop();

    void setRunning(bool running);
    bool getRunning();

private:
    std::queue<AVPacket*> m_queue;
    QMutex m_mutex;
    QWaitCondition m_notEmpty;
    QWaitCondition m_notFull;
    bool m_running;
};


#endif // PACKETQUEUE_H