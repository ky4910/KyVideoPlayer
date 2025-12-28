#ifndef VIDEOOUTPUTRENDERER_H
#define VIDEOOUTPUTRENDERER_H

#include <QMutex>
#include <QQuickFramebufferObject>

#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>

extern "C"
{
    #include <libavutil/frame.h>
}

class VideoOutputRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions
{

public:
    void render() override;
    void synchronize(QQuickFramebufferObject* item) override;
    void initGL();

    virtual ~VideoOutputRenderer();

private:
    bool mInitialized = false;
    GLuint texY, texU, texV;
    QMutex mutex;
    AVFrame* currentFrame = nullptr;
    QOpenGLShaderProgram *mShaderProgram;
};

#endif // VIDEOOUTPUTRENDERER_H
