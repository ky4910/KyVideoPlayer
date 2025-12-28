#include "videooutputfbo.h"
#include "videooutputrenderer.h"
#include "utils.h"

VideoOutputFBO::VideoOutputFBO(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
{
    qDebugT() << "VideoOutputFBO::VideoOutputFBO called.";
    setFlag(ItemHasContents, true);
}

QQuickFramebufferObject::Renderer *VideoOutputFBO::createRenderer() const
{
    qDebugT() << "VideoOutputFBO::createRenderer called.";
    return new VideoOutputRenderer();
}

void VideoOutputFBO::processYUV(AVFrame *frame)
{
    QMutexLocker locker(&m_mutex);

    if (!m_frame)
    {
        m_frame = av_frame_alloc();
    }

    av_frame_unref(m_frame);
    av_frame_ref(m_frame, frame);

    av_frame_free(&frame);

    update();

    // qDebugT() << "VideoOutputFBO::processYUV called.";
}

