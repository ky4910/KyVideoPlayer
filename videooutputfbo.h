#ifndef VIDEOOUTPUTFBO_H
#define VIDEOOUTPUTFBO_H

#include <QMutex>
#include <QQuickFramebufferObject>

extern "C"
{
    #include <libavutil/frame.h>
}

class VideoOutputFBO : public QQuickFramebufferObject
{
    Q_OBJECT

public:
    explicit VideoOutputFBO(QQuickItem *parent = nullptr);
    Renderer *createRenderer() const override;

    QMutex m_mutex;
    AVFrame* m_frame = nullptr;

public slots:
    void processYUV(AVFrame *frame);

signals:
    void frameReady();

};

#endif // VIDEOOUTPUTFBO_H
