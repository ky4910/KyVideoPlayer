#ifndef UTILS_H
#define UTILS_H

#include <QDebug>
#include <QThread>

#define qDebugT() (qDebug() << QThread::currentThread()->objectName() << " " << QThread::currentThreadId() << " ")

class Utils
{
public:
    Utils();
};


#endif // UTILS_H
