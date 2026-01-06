#ifndef VIDEOOUTPUTFBO_H
#define VIDEOOUTPUTFBO_H

#include <QMutex>
#include <QQuickFramebufferObject>

#include "playstate.h"

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

    Q_INVOKABLE void clearScreen();

    QMutex m_mutex;
    AVFrame* m_frame = nullptr;

    void setPlayState(PlayState state);

public slots:
    void processYUV(AVFrame *frame);

signals:
    void frameReady();

private:
    PlayState m_state;
};

#endif // VIDEOOUTPUTFBO_H
