#include <stdio.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "../include/Game.h"

int main(int argc, char* args[])
{
    Game game;
    if (!game.init("./textures/", "./fonts/")) return -1;
    game.play();

    return 0;
}