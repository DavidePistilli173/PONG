#pragma once

#include <SDL.h>

class LTimer
{
public:
    LTimer();

    void start();
    void pause();
    void resume();
    void stop();

    Uint32 getTicks();

    bool isStarted();
    bool isPaused();

private:
    Uint32 _startTicks;
    Uint32 _pausedTicks;

    bool _paused;
    bool _started;
};

