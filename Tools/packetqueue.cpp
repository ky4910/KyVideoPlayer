#include "packetqueue.h"
#include "utils.h"

PacketQueue::PacketQueue()
{
    m_running = true;
    m_aborted = false;
}

void PacketQueue::push(AVPacket* pkt)
{
    QMutexLocker locker(&m_mutex);

    while (!m_aborted && m_queue.size() >= MAX_QUEUE_SIZE)
    {
        m_notFull.wait(&m_mutex);
    }

    if (m_aborted)
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

    while (m_queue.empty() && !m_aborted)
    {
        m_notEmpty.wait(&m_mutex);
    }

    if (m_aborted)
        return nullptr;

    AVPacket* pkt = m_queue.front();
    m_queue.pop();

    m_notFull.wakeOne();
    return pkt;
}

void PacketQueue::abort()
{
    QMutexLocker locker(&m_mutex);
    m_aborted = true;

    m_notEmpty.wakeAll();
    m_notFull.wakeAll();
}

void PacketQueue::clear()
{
    QMutexLocker locker(&m_mutex);

    while (!m_queue.empty())
    {
        AVPacket* pkt = m_queue.front();
        m_queue.pop();
        av_packet_free(&pkt);
    }
}

bool PacketQueue::empty()
{
    QMutexLocker locker(&m_mutex);
    return m_queue.empty();
}

void PacketQueue::setRunning(bool running)
{
    m_running = running;
}

bool PacketQueue::getRunning()
{
    return m_running;
}

