#include "packetqueue.h"

PacketQueue::PacketQueue()
{
    m_running = true;
}

void PacketQueue::push(AVPacket* pkt)
{
    QMutexLocker locker(&m_mutex);

    while (m_queue.size() >= MAX_QUEUE_SIZE && m_running)
    {
        m_notFull.wait( &m_mutex );
    }

    if (!m_running)
    {
        av_packet_free(&pkt);
        return;
    }

    m_queue.push(pkt);
    m_notEmpty.wakeOne();
}

AVPacket* PacketQueue::pop()
{
    QMutexLocker locker(&m_mutex);

    while (m_queue.empty())
    {
        m_notEmpty.wait(&m_mutex);
    }

    if (!m_running)
        return nullptr;

    AVPacket* pkt = m_queue.front();
    m_queue.pop();
    m_notFull.wakeOne();

    return pkt;
}

void PacketQueue::setRunning(bool running)
{
    m_running = running;
}

bool PacketQueue::getRunning()
{
    return m_running;
}
