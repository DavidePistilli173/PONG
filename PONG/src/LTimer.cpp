#include "../include/LTimer.h"

LTimer::LTimer() :
    _startTicks(0),
    _pausedTicks(0),
    _started(false),
    _paused(false)
{}

void LTimer::start()
{
    if (!_started)
    {
        _startTicks = SDL_GetTicks();
        _started = true;
    }
}

void LTimer::pause()
{
    if (_started && !_paused)
    {
        _pausedTicks = getTicks();
        _paused = true;
    }
}

void LTimer::resume()
{
    if (_paused)
    {
        _startTicks = SDL_GetTicks();
        _paused = false;
    }
}

void LTimer::stop()
{
    if (_started)
    {
        _pausedTicks = 0;
        _started = false;
        _paused = false;
    }
}

Uint32 LTimer::getTicks()
{
    if (_started && !_paused) return (SDL_GetTicks() - _startTicks) + _pausedTicks;
    if (_paused) return _pausedTicks;
    return 0;
}

bool LTimer::isStarted()
{
    return _started;
}

bool LTimer::isPaused()
{
    return _paused;
}
