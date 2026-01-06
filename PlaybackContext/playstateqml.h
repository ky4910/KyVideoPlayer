#ifndef PLAYSTATEQML_H
#define PLAYSTATEQML_H

#pragma once
#include <QObject>
#include "playstate.h"

class PlayStateQml : public QObject
{
    Q_OBJECT

public:
    enum PlayStateEnum
    {
        Idle     = static_cast<int>(PlayState::Idle),
        Playing  = static_cast<int>(PlayState::Playing),
        Paused   = static_cast<int>(PlayState::Paused),
        Stopping = static_cast<int>(PlayState::Stopping),
        Stopped = static_cast<int>(PlayState::Stopped)
    };
    Q_ENUM(PlayStateEnum)
};

#endif // PLAYSTATEQML_H
