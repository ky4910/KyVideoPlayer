#include "videooutputrenderer.h"
#include "utils.h"
#include "videooutputfbo.h"
#include <QFile>
#include <QOpenGLFramebufferObject>

VideoOutputRenderer::~VideoOutputRenderer()
{
    if ( currentFrame )
        av_frame_free(&currentFrame);
}

void VideoOutputRenderer::initGL()
{
    mShaderProgram = new QOpenGLShaderProgram();

    qDebug() << "file PATH: " << QFile("D:/Code/QtRelated/KyVideoPlayer/yuv.vert").exists();

    mShaderProgram->addShaderFromSourceFile(
        QOpenGLShader::Vertex, "D:/Code/QtRelated/KyVideoPlayer/yuv.vert");

    mShaderProgram->addShaderFromSourceFile(
        QOpenGLShader::Fragment, "D:/Code/QtRelated/KyVideoPlayer/yuv.frag");

    if (!mShaderProgram->link()) {
        qWarning() << "Shader link failed:" << mShaderProgram->log();
    }

    // Create textures for Y, U, V planes
    glGenTextures(1, &texY);
    glGenTextures(1, &texU);
    glGenTextures(1, &texV);
}


void VideoOutputRenderer::render()
{
    qDebugT() << "VideoOutputRenderer::render called.";

    if ( !currentFrame || currentFrame->width == 0)
    {
        qDebugT() << "No frame to render.";
        return;
    }

    if ( !mInitialized )
    {
        initializeOpenGLFunctions();
        initGL();
        mInitialized = true;
    }

    QSize size = framebufferObject()->size();
    glViewport(0, 0, size.width(), size.height());

    // glViewport(0, 0, w, h);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 1. Upload YUV data to textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texY);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, currentFrame->linesize[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, currentFrame->width, currentFrame->height,
                 0, GL_RED, GL_UNSIGNED_BYTE, currentFrame->data[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texU);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, currentFrame->linesize[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, currentFrame->width/2, currentFrame->height/2,
                 0, GL_RED, GL_UNSIGNED_BYTE, currentFrame->data[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texV);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, currentFrame->linesize[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, currentFrame->width/2, currentFrame->height/2,
                 0, GL_RED, GL_UNSIGNED_BYTE, currentFrame->data[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Reset row length
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    const int w = currentFrame->width;
    const int h = currentFrame->height;

    // qDebug() << "pix_fmt:" << currentFrame->format;
    // qDebug() << "width:" << w << "height:" << h;
    float viewW = size.width();
    float viewH = size.height();

    qDebugT() << "viewW:" << viewW << "viewH:" << viewH;

    float videoAspect = float(w) / h;
    float viewAspect  = viewW / viewH;

    float scaleX = 1.0f;
    float scaleY = 1.0f;

    if (viewAspect > videoAspect)
        scaleX = videoAspect / viewAspect;
    else
        scaleY = viewAspect / videoAspect;

    // 2. Draw full-screen quad, use shader to convert YUV → RGB
    mShaderProgram->bind();
    mShaderProgram->setUniformValue("texY", 0);
    mShaderProgram->setUniformValue("texU", 1);
    mShaderProgram->setUniformValue("texV", 2);
    mShaderProgram->setUniformValue("uScale", QVector2D(scaleX, scaleY));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    mShaderProgram->release();

    // 3. Request next frame
    // update();
}

void VideoOutputRenderer::synchronize(QQuickFramebufferObject* item)
{
    VideoOutputFBO *fboItem = static_cast<VideoOutputFBO*>(item);

    QMutexLocker locker(&fboItem->m_mutex);

    if (fboItem->m_frame)
    {
        if ( !currentFrame )
            currentFrame = av_frame_alloc();

        av_frame_ref(currentFrame, fboItem->m_frame);
    }
}

