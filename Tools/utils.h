#ifndef UTILS_H
#define UTILS_H

#include <QDebug>
#include <QString>
#include <QThread>
#include <QDateTime>

// #define qDebugT() (qDebug() << QThread::currentThread()->objectName() << " " << QThread::currentThreadId() << " ")

inline QString padRight(const QString &str, int width, QChar fill = ' ') {
    if (str.length() >= width) return str;
    return str + QString(width - str.length(), fill);
}

inline QString padLeft(const QString &str, int width, QChar fill = ' ') {
    if (str.length() >= width) return str;
    return QString(width - str.length(), fill) + str;
}

#define qDebugT() \
(qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") \
          << padRight(QThread::currentThread()->objectName(), 20) \
          << padLeft(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()), 16), 8, '0') \
          << " ")

class Utils
{
public:
    Utils();
};


#endif // UTILS_H
