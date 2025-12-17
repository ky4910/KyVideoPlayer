#ifndef MINEDECODER_H
#define MINEDECODER_H

#include <QObject>
#include <QDebug>

class MineDecoder : public QObject
{
    Q_OBJECT

public:
    explicit MineDecoder(QObject *parent = nullptr)  : QObject(parent) {}

public slots:
    int startDecoding(const QString &filePath);
};

#endif // MINEDECODER_H
