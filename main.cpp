#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>

#include "utils.h"
// #include "minedecoder.h"
#include "playercontroller.h"
#include "videooutputfbo.h"

extern "C"
{
    #include <libavformat/version.h>
    #include <libavcodec/version.h>
    #include <libswscale/version.h>
    #include <libswresample/version.h>
    #include <libavutil/version.h>
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    // TO-DO: Should use relative path
    const QUrl url(QStringLiteral("file:///D:/Code/QtRelated/KyVideoPlayer/Main.qml"));

    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    qDebugT() << "Qt build version:" << QT_VERSION_STR;
    qDebugT() << "Qt runtime version:" << qVersion();

    // Set QML module load way
    QCoreApplication::setLibraryPaths(QStringList() << ":/qt/qml/");

    PlayerController *playerController = new PlayerController;
    // playerController->moveToThread(thread);
    // thread->start();

    qmlRegisterType<VideoOutputFBO>("CustomFBO", 1, 0, "VideoOutputFBO");

    qDebugT() << "avformat version: " << LIBAVFORMAT_IDENT;
    qDebugT() << "avcodec version: " << LIBAVCODEC_IDENT;
    qDebugT() << "swscale version: " << LIBSWSCALE_IDENT;
    qDebugT() << "swresample version: " << LIBSWRESAMPLE_IDENT;
    qDebugT() << "avutil version: " << LIBAVUTIL_IDENT;

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.rootContext()->setContextProperty("playerController", playerController);

    engine.load(url);

    return app.exec();
}

