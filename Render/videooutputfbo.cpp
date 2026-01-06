#include "videooutputfbo.h"
#include "videooutputrenderer.h"
#include "utils.h"

VideoOutputFBO::VideoOutputFBO(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
{
    qDebugT() << "VideoOutputFBO::VideoOutputFBO called.";
    setFlag(ItemHasContents, true);
    m_state = PlayState::Idle;
}

QQuickFramebufferObject::Renderer *VideoOutputFBO::createRenderer() const
{
    qDebugT() << "VideoOutputFBO::createRenderer called.";
    return new VideoOutputRenderer();
}

void VideoOutputFBO::processYUV(AVFrame *frame)
{
    // if ( m_state == PlayState::Stopped )
    // {
    //     av_frame_free(&frame);
    //     return;
    // }

    if ( m_state != PlayState::Playing )
    {
        // av_frame_free(&frame);
        return;
    }

    QMutexLocker locker(&m_mutex);

    if (!m_frame)
    {
        m_frame = av_frame_alloc();
    }

    av_frame_unref(m_frame);
    av_frame_ref(m_frame, frame);

    av_frame_free(&frame);

    if ( m_state != PlayState::Stopped )
    {
        update();
    }

    // qDebugT() << "VideoOutputFBO::processYUV called.";
}

void VideoOutputFBO::setPlayState(PlayState state)
{
    QMutexLocker locker(&m_mutex);
    m_state = state;

    if (state == PlayState::Stopped)
    {
        if (m_frame)
        {
            av_frame_free(&m_frame);
            m_frame = nullptr;
        }
        update();
    }
}

void VideoOutputFBO::clearScreen()
{
    QMutexLocker locker(&m_mutex);

    if (m_frame)
    {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }

    update();

    qDebugT() << "VideoOutputFBO::clearScreen called.";
}
