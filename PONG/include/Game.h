#pragma once

#include "LTexture.h"

/*
    Playere movement status for the current frame.
    -1 -> player moved up, 0 -> player didn't move, 1 -> player moved down.
*/
enum class PlayerMoved
{
    UP = -1,
    NA,
    DOWN
};

class Game
{
public:
    /* Window constants. */
    const std::string GAME_NAME = "PONG"; /* Name of the game. */
    const std::string SCORE_SEPARATOR = ":"; /* Separator of the two scores. */

    const int START_X = 0; /* Initial x coordinate of the window. */
    const int START_Y = 0; /* Initial y coordinate of the window. */
    const int START_WIDTH = 800; /* Initial width of the window. */
    const int START_HEIGHT = 600; /* Initial height of the window. */
    const int FONT_SIZE = 100; /* Font size for in-game text. */
    const int BORDER_OFFSET = 45; /* Offset of the table border from the left margin. */
    const int BORDER_WIDTH = 15; /* Width of the border line. */
    const int GOAL_WIDTH = 15; /* Width of the goal line. */
    const int GOAL_OFFSET = 404; /* Offset of the goal line from the table border. */
    const int PAD_BORDER = 10; /* Shadow border of the player pad. */
    const int UPPER_MARGIN = 30; /* Margin from the top of the screen. */
    const int SEPARATOR_MARGIN = 20; /* Margin from the score separator. */
    const int BALL_MARGIN = 8; /* Margin from the surface of the ball. */

    const Uint8 DEFAULT_RED = 0; /* Initial red component of default colour. */
    const Uint8 DEFAULT_GREEN = 0; /* Initial green component of default colour. */
    const Uint8 DEFAULT_BLUE = 0; /* Initial blue component of default colour. */
    const Uint8 DEFAULT_ALPHA = 255; /* Initial alpha component of default colour. */

    const float TABLE_COEFF = 0.9f; /* Percentage of the vertical space that is occupied by the playing field. */
    const float SCORE_HCOEFF = 0.2f; /* Percentage of the horizontal space that is occupied by the score text. */
    const float PLAYER_SPEED_COEFF = 0.01f; /* Percentage of the screen covered in one frame by the player. */
    const float BALL_SPEED_COEFF = 0.005f; /* Percentage of the horizontal space covered in one frame by the player. */

    const SDL_Color SCORE_TEXT_COLOUR = { 255, 255, 255 }; /* Colour of the score text. */

    const Uint32 MIN_FRAME_TIME = 10; /* Minimum frame time. */


    /* Names of the required media files. */
    const std::string FONT_NAME = "lazy.ttf";
    const std::string BACKGROUND_NAME = "table.png";
    const std::string PAD_NAME = "pad.png";
    const std::string BALL_NAME = "ball.png";

    /* Constructor - Destructor. */
    Game();
    ~Game();

    bool init(std::string texturePath, std::string fontPath); /* Load required data and initialize the game. */
    void play(); /* Play the game. */

private:
    /* Window variables. */
    SDL_Window* _window; /* Main window of the game. */
    SDL_Renderer* _renderer; /* Main renderer of the game. */
    int _wWidth; /* Window width. */
    int _wHeight; /* Window height. */

    /* Texture data. */
    LTexture _background; /* Playing field background. */
    LTexture _pad; /* Player pad. */
    LTexture _ball; /* The ball. */
    LTexture _colon; /* Colon separating the two scores. */
    LTexture _leftScoreText; /* Text of the left score. */
    LTexture _rightScoreText; /* Text of the right score. */

    SDL_Rect _backGroundDest; /* Background destination rectangle. */
    TTF_Font* _font; /* Main game font. */

    /* Player variables. */
    SDL_Rect _leftPlayer; /* Position of the left pad. */
    SDL_Rect _rightPlayer; /* Position of the right pad. */
    SDL_Rect _leftPlayerLock; /* Left player reset position. */
    SDL_Rect _rightPlayerLock; /* Right player reset position. */
    int _playerUpperLimit; /* Minimum y coordinate of the player. */
    int _playerLowerLimit; /* Maximum y coordinate of the player. */
    int _leftScore; /* Score for the left player. */
    int _rightScore; /* Score fot the right player. */
    PlayerMoved _leftPlayerMoved; 
    PlayerMoved _rightPlayerMoved;
    float _playerSpeed; /* Player speed in pixels per second. */

    /* Ball variables. */
    SDL_Rect _ballPosition; /* Ball current position. */
    SDL_Rect _leftPlayerBallLock; /* Ball left reset position. */
    SDL_Rect _rightPlayerBallLock; /* Ball right reset position. */
    int _ballDefaultSpeed; /* Default horizontal speed of the ball. */
    int _ballSpeedX; /* Ball speed on x axis. */
    int _ballSpeedY; /* Ball speed on y axis. */
    int _leftGoal; /* Left goal x coordinate. */
    int _rightGoal; /* Right goal x coordinate. */
    int _ballLowerLimit; /* Lower border for the ball. */
    bool _lock; /* Is the ball locked to a player? */
    bool _lockSide; /* False -> ball to the left, True -> ball to the right. */

    /* Score position and size. */
    SDL_Rect _leftScoreRect;
    SDL_Rect _rightScoreRect;
    SDL_Rect _separatorRect;
    float _textScale;

    void render(); /* Render the game. */
};