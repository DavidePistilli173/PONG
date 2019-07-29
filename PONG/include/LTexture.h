#pragma once

#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

class LTexture
{
public:
    LTexture();
    ~LTexture();

    void freeTexture();

    bool loadFromFile(SDL_Renderer* renderer, std::string path);
    bool loadFromRenderedText(SDL_Renderer* renderer, std::string textureText, SDL_Color textColour, TTF_Font* font);

    void render(SDL_Renderer* renderer, 
                int x, 
                int y, 
                SDL_Rect* clip = nullptr, 
                SDL_Rect* destRect = nullptr, 
                double angle = 0.0, 
                SDL_Point * centre = nullptr, 
                SDL_RendererFlip flip = SDL_FLIP_NONE);

    void setColour(Uint8 red, Uint8 green, Uint8 blue);
    void setBlendMode(SDL_BlendMode blending);
    void setAlpha(Uint8 alpha);

    int getWidth();
    int getHeight();
private:
    SDL_Texture* _texture;
    int _width, _height;
};

