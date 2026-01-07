#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#pragma once

enum class PlayState {
    Idle,
    Playing,
    Paused,
    Stopping,
    Stopped,
    Finished
};

#endif // PLAYSTATE_H
