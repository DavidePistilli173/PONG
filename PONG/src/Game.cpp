#include "../include/Game.h"

#include <stdio.h>
#include <sstream>

#include <SDL_image.h>

#include "../include/LTimer.h"

Game::Game() :
    _window(nullptr),
    _renderer(nullptr),
    _wWidth(0),
    _wHeight(0),
    _backGroundDest({ 0,0,0,0 }),
    _font(nullptr),
    _leftPlayer({ 0,0,0,0 }),
    _rightPlayer({ 0,0,0,0 }),
    _leftPlayerLock({ 0,0,0,0 }),
    _rightPlayerLock({ 0,0,0,0 }),
    _leftScore(0),
    _rightScore(0),
    _leftPlayerMoved(PlayerMoved::NA),
    _rightPlayerMoved(PlayerMoved::NA),
    _leftScoreRect({ 0,0,0,0 }),
    _rightScoreRect({ 0,0,0,0 }),
    _separatorRect({ 0,0,0,0 }),
    _textScale(0.0f),
    _ballPosition({ 0,0,0,0 }),
    _leftPlayerBallLock({ 0,0,0,0 }),
    _rightPlayerBallLock({ 0,0,0,0 }),
    _ballSpeedX(0.0f),
    _ballSpeedY(0.0f),
    _leftGoal(0),
    _rightGoal(0),
    _ballLowerLimit(0),
    _lock(true),
    _lockSide(false)
{}

Game::~Game()
{
    /* Free resources. */
    TTF_CloseFont(_font);
    _font = nullptr;
    SDL_DestroyRenderer(_renderer);
    _renderer = nullptr;
    SDL_DestroyWindow(_window);
    _window = nullptr;

    /* Quit SDL subsystems. */
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

bool Game::init(std::string texturePath, std::string fontPath)
{
    /* Init of SDL subsystems. */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("%s", SDL_GetError());
        return false;
    }
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        printf("%s", IMG_GetError());
        return false;
    }
    if (TTF_Init() == -1)
    {
        printf("%s", TTF_GetError());
        return false;
    }

    /* Create a fullscreen window. */
    _window = SDL_CreateWindow(GAME_NAME.c_str(), START_X, START_Y, START_WIDTH, START_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (_window == nullptr)
    {
        printf("%s", SDL_GetError());
        return false;
    }
    SDL_GetWindowSize(_window, &_wWidth, &_wHeight);

    /* Create a renderer for the window. */
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (_renderer == nullptr)
    {
        printf("%s", SDL_GetError());
        return false;
    }
    SDL_SetRenderDrawColor(_renderer, DEFAULT_RED, DEFAULT_GREEN, DEFAULT_BLUE, DEFAULT_ALPHA);

    /* Load the main font. */
    _font = TTF_OpenFont((fontPath + FONT_NAME).c_str(), FONT_SIZE);
    if (_font == nullptr)
    {
        printf("%s", TTF_GetError());
        return false;
    }

    /* Load textures. */
    if (!_background.loadFromFile(_renderer, texturePath + BACKGROUND_NAME)) return false;
    if (!_pad.loadFromFile(_renderer, texturePath + PAD_NAME)) return false;
    if (!_ball.loadFromFile(_renderer, texturePath + BALL_NAME)) return false;
    if (!_colon.loadFromRenderedText(_renderer, SCORE_SEPARATOR, SCORE_TEXT_COLOUR, _font)) return false;
    if (!_leftScoreText.loadFromRenderedText(_renderer, std::to_string(_leftScore), SCORE_TEXT_COLOUR, _font)) return false;
    if (!_rightScoreText.loadFromRenderedText(_renderer, std::to_string(_rightScore), SCORE_TEXT_COLOUR, _font)) return false;

    /* Set size and position of the background. */
    _backGroundDest.w = _wWidth;
    _backGroundDest.h = TABLE_COEFF * _wHeight;
    _backGroundDest.y = _wHeight - _backGroundDest.h;

    /* Set size and position of the players. */
    float hScale = static_cast<float>(_wWidth) / _background.getWidth();
    float vScale = static_cast<float>(_backGroundDest.h) / _background.getHeight();
    _leftPlayer.w = _pad.getWidth() * hScale;
    _rightPlayer.w = _leftPlayer.w;
    _leftPlayer.h = _pad.getHeight() * vScale;
    _rightPlayer.h = _leftPlayer.h;
    
    int offset = (BORDER_OFFSET + BORDER_WIDTH + GOAL_OFFSET + (GOAL_WIDTH >> 1)) * hScale;
    _leftPlayer.x = offset - (_leftPlayer.w >> 1);
    _rightPlayer.x = (_wWidth - offset) - (_rightPlayer.w >> 1);
    _leftPlayer.y = _backGroundDest.y + (_backGroundDest.h >> 1);
    _rightPlayer.y = _leftPlayer.y;

    _leftPlayerLock.h = _leftPlayer.h;
    _leftPlayerLock.w = _leftPlayer.w;
    _leftPlayerLock.y = _leftPlayer.y;
    _leftPlayerLock.x = _leftPlayer.x;

    _rightPlayerLock.h = _rightPlayer.h;
    _rightPlayerLock.w = _rightPlayer.w;
    _rightPlayerLock.y = _rightPlayer.y;
    _rightPlayerLock.x = _rightPlayer.x;

    _playerSpeed = PLAYER_SPEED_COEFF * _wHeight;
    _playerUpperLimit = _backGroundDest.y + (BORDER_OFFSET + BORDER_WIDTH) * vScale;
    _playerLowerLimit = _wHeight - (BORDER_OFFSET + BORDER_WIDTH + _pad.getHeight() + PAD_BORDER) * vScale;

    /* Set position and speed of the ball. */
    _ballSpeedX = 0;
    _ballSpeedY = 0;
    _ballDefaultSpeed = _wWidth * BALL_SPEED_COEFF;

    _leftPlayerBallLock.h = _ball.getHeight() * vScale;
    _leftPlayerBallLock.w = _ball.getWidth() * vScale;
    _leftPlayerBallLock.y = _leftPlayer.y + ((_leftPlayer.h - _ballPosition.h) >> 1);
    _leftPlayerBallLock.x = _leftPlayer.x + (PAD_BORDER + _pad.getWidth() - BALL_MARGIN) * hScale;

    _rightPlayerBallLock.h = _leftPlayerBallLock.h;
    _rightPlayerBallLock.w = _leftPlayerBallLock.w;
    _rightPlayerBallLock.y = _leftPlayerBallLock.y;
    _rightPlayerBallLock.x = _rightPlayer.x - ((_pad.getWidth() + (BALL_MARGIN << 1)) * hScale);

    _ballPosition.h = _leftPlayerBallLock.h;
    _ballPosition.w = _leftPlayerBallLock.w;
    _ballPosition.y = _leftPlayerBallLock.y;
    _ballPosition.x = _leftPlayerBallLock.x;

    _ballLowerLimit = _wHeight - (BORDER_OFFSET + BORDER_WIDTH + _ballPosition.h) * vScale;

    /* Set the goal positions. */
    _leftGoal = (BORDER_OFFSET + BORDER_WIDTH + GOAL_OFFSET + GOAL_WIDTH - BALL_MARGIN) * hScale;
    _rightGoal = _wWidth - _leftGoal;

    /* Set position and size of the score text. */
    _separatorRect.h = ((1 - TABLE_COEFF) / 2) * _wHeight;
    _separatorRect.y = UPPER_MARGIN;
    _separatorRect.w = _colon.getWidth();
    _separatorRect.x = (_wWidth - _colon.getWidth()) >> 1;

    _textScale = static_cast<float>(_separatorRect.h) / _colon.getHeight();

    _leftScoreRect.h = _separatorRect.h;
    _rightScoreRect.h = _separatorRect.h;
    _leftScoreRect.y = _separatorRect.y;
    _rightScoreRect.y = _separatorRect.y;
    _leftScoreRect.w = _leftScoreText.getWidth() * _textScale;
    _rightScoreRect.w = _leftScoreRect.w;
    _leftScoreRect.x = _separatorRect.x - SEPARATOR_MARGIN;
    _rightScoreRect.x = _separatorRect.x + _separatorRect.w + SEPARATOR_MARGIN;

    return true;
}

void Game::play()
{
    LTimer elapsedTime;
    SDL_Event event;
    bool done = false;
    while (!done)
    {
        /* If the game is running too fast, slow it down. */
        elapsedTime.stop();
        Uint32 deltaSeconds = elapsedTime.getTicks();
        if (deltaSeconds < MIN_FRAME_TIME)
        {
            SDL_Delay(MIN_FRAME_TIME - deltaSeconds);
        }
        elapsedTime.start();

        /* Input handling. */
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    done = true;
                }
            }
        }

        _leftPlayerMoved = PlayerMoved::NA;
        _rightPlayerMoved = PlayerMoved::NA;
        const Uint8* currentKeyState = SDL_GetKeyboardState(nullptr);
        /* Left player controls. */
        if (currentKeyState[SDL_SCANCODE_W])
        {
            if (_leftPlayer.y > _playerUpperLimit)
            {
                _leftPlayerMoved = PlayerMoved::UP;
                float diff = (_leftPlayer.y - _playerUpperLimit) - _playerSpeed;
                float choice = ((diff < 0) ? diff : _playerSpeed);
                _leftPlayer.y -= choice;
                if (_lock && !_lockSide)
                {
                    if (currentKeyState[SDL_SCANCODE_RETURN])
                    {
                        _lock = false;
                        _ballSpeedX = _ballDefaultSpeed;
                        _ballSpeedY = -_playerSpeed;
                    }
                    else
                    {
                        _ballPosition.y -= choice;
                    }
                }
            }
        }
        else if (currentKeyState[SDL_SCANCODE_S])
        {
            if (_leftPlayer.y < _playerLowerLimit)
            {
                _leftPlayerMoved = PlayerMoved::DOWN;
                float diff = _playerSpeed - (_playerLowerLimit - _leftPlayer.y);
                float choice = ((diff < 0) ? _playerSpeed : diff);
                _leftPlayer.y += choice;
                if (_lock && !_lockSide)
                {
                    if (currentKeyState[SDL_SCANCODE_RETURN])
                    {
                        _lock = false;
                        _ballSpeedX = _ballDefaultSpeed;
                        _ballSpeedY = _playerSpeed;
                    }
                    else
                    {
                        _ballPosition.y += choice;
                    }
                }
            }
        }
        /* Right player controls. */
        if (currentKeyState[SDL_SCANCODE_UP])
        {
            if (_rightPlayer.y > _playerUpperLimit)
            {
                _rightPlayerMoved = PlayerMoved::UP;
                float diff = (_rightPlayer.y - _playerUpperLimit) - _playerSpeed;
                float choice = ((diff < 0) ? diff : _playerSpeed);
                _rightPlayer.y -= choice;
                if (_lock && _lockSide)
                {
                    if (currentKeyState[SDL_SCANCODE_RETURN])
                    {
                        _lock = false;
                        _ballSpeedX = -_ballDefaultSpeed;
                        _ballSpeedY = -_playerSpeed;
                    }
                    else
                    {
                        _ballPosition.y -= choice;
                    }
                }
            }
        }
        else if (currentKeyState[SDL_SCANCODE_DOWN])
        {
            if (_rightPlayer.y < _playerLowerLimit)
            {
                _rightPlayerMoved = PlayerMoved::DOWN;
                float diff = _playerSpeed - (_playerLowerLimit - _rightPlayer.y);
                float choice = ((diff < 0) ? _playerSpeed : diff);
                _rightPlayer.y += choice;
                if (_lock && _lockSide)
                {
                    if (currentKeyState[SDL_SCANCODE_RETURN])
                    {
                        _lock = false;
                        _ballSpeedX = -_ballDefaultSpeed;
                        _ballSpeedY = _playerSpeed;
                    }
                    else
                    {
                        _ballPosition.y += choice;
                    }
                }
            }
        }

        if (_lock && currentKeyState[SDL_SCANCODE_RETURN])
        {
            if (!_lockSide) _ballSpeedX = _ballDefaultSpeed;
            else _ballSpeedX = -_ballDefaultSpeed;
            _lock = false;
        }

        /* Ball movement. */
        _ballPosition.y += _ballSpeedY;
        _ballPosition.x += _ballSpeedX;

        /* If the ball hits a border, reverse its vertical speed. */
        if (!_lock)
        {
            if (_ballPosition.y <= _playerUpperLimit && _ballSpeedY < 0)
            {
                _ballSpeedY = -_ballSpeedY;
            }
            else if (_ballPosition.y >= _ballLowerLimit && _ballSpeedY > 0)
            {
                _ballSpeedY = -_ballSpeedY;
            }

            /* If the ball hits a player pad, compute the collision. */
            if (_ballPosition.x <= _leftPlayer.x + _leftPlayer.w &&
                _ballPosition.y >= _leftPlayer.y &&
                _ballPosition.y <= _leftPlayer.y + _leftPlayer.h &&
                _ballSpeedX < 0)
            {
                _ballSpeedX = -_ballSpeedX;
                _ballSpeedY = _ballSpeedY + _playerSpeed * static_cast<int>(_leftPlayerMoved);
                if (std::abs(_ballSpeedY) > _ballDefaultSpeed) _ballSpeedY = _ballDefaultSpeed;
            }
            else if (_ballPosition.x + _ballPosition.w >= _rightPlayer.x &&
                     _ballPosition.y >= _rightPlayer.y &&
                     _ballPosition.y <= _rightPlayer.y + _rightPlayer.h &&
                     _ballSpeedX > 0)
            {
                _ballSpeedX = -_ballSpeedX;
                _ballSpeedY = _ballSpeedY + _playerSpeed * static_cast<int>(_rightPlayerMoved);
                if (std::abs(_ballSpeedY) > _ballDefaultSpeed) _ballSpeedY = _ballDefaultSpeed;
            }

            /* If the ball hits a goal, score and reset. */
            if (_ballPosition.x < _leftGoal)
            {
                _lock = true;
                _lockSide = true;
                _ballSpeedX = 0;
                _ballSpeedY = 0;
                ++_rightScore;
                _rightScoreText.loadFromRenderedText(_renderer, std::to_string(_rightScore), SCORE_TEXT_COLOUR, _font);
                _leftPlayer = _leftPlayerLock;
                _rightPlayer = _rightPlayerLock;
                _ballPosition = _rightPlayerBallLock;
            }
            else if (_ballPosition.x > _rightGoal)
            {
                _lock = true;
                _lockSide = false;
                _ballSpeedX = 0;
                _ballSpeedY = 0;
                ++_leftScore;
                _leftScoreText.loadFromRenderedText(_renderer, std::to_string(_leftScore), SCORE_TEXT_COLOUR, _font);
                _leftPlayer = _leftPlayerLock;
                _rightPlayer = _rightPlayerLock;
                _ballPosition = _leftPlayerBallLock;
            }
        }

        /* Render the current frame. */
        render();
    }
}

void Game::render()
{
    SDL_RenderClear(_renderer);
    _background.render(_renderer, _backGroundDest.x, _backGroundDest.y, nullptr, &_backGroundDest);
    _pad.render(_renderer, _leftPlayer.x, _leftPlayer.y, nullptr, &_leftPlayer);
    _pad.render(_renderer, _rightPlayer.x, _rightPlayer.y, nullptr, &_rightPlayer);
    _ball.render(_renderer, _ballPosition.x, _ballSpeedY, nullptr, &_ballPosition);
    _colon.render(_renderer, _separatorRect.x, _separatorRect.y, nullptr, &_separatorRect);
    _leftScoreText.render(_renderer, _leftScoreRect.x, _leftScoreRect.y, nullptr, &_leftScoreRect);
    _rightScoreText.render(_renderer, _rightScoreRect.x, _rightScoreRect.y, nullptr, &_rightScoreRect);
    SDL_RenderPresent(_renderer);
}
