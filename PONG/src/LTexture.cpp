#include "../include/LTexture.h"

#include <SDL_image.h>

LTexture::LTexture() :
    _texture(nullptr),
    _width(0),
    _height(0)
{};
LTexture::~LTexture()
{
    freeTexture();
}

void LTexture::freeTexture()
{
    if (_texture != nullptr)
    {
        SDL_DestroyTexture(_texture);
        _texture = nullptr;
        _width = 0;
        _height = 0;
    }
}

bool LTexture::loadFromFile(SDL_Renderer* renderer, std::string path)
{
    SDL_Surface* tempSurface = IMG_Load(path.c_str());
    if (tempSurface == nullptr)
    {
        printf("%s", IMG_GetError());
        return false;
    }
    SDL_SetColorKey(tempSurface, SDL_TRUE, SDL_MapRGB(tempSurface->format, 0, 255, 255));

    _texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    if (_texture == nullptr)
    {
        printf("%s", SDL_GetError());
        return false;
    }
    _width = tempSurface->w;
    _height = tempSurface->h;
    return true;
}

bool LTexture::loadFromRenderedText(SDL_Renderer* renderer, std::string textureText, SDL_Color textColour, TTF_Font* font)
{
    freeTexture();

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColour);
    if (textSurface == nullptr)
    {
        printf("%s", TTF_GetError());
        return false;
    }

    _texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    _width = textSurface->w;
    _height = textSurface->h;
    SDL_FreeSurface(textSurface);
    if (_texture == nullptr)
    {
        printf("%s", SDL_GetError());
        return false;
    }
    return true;
}

void LTexture::render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip, SDL_Rect* destRect, double angle, SDL_Point * centre, SDL_RendererFlip flip)
{
    SDL_RenderCopyEx(renderer, _texture, clip, destRect, angle, centre, flip);
}

void LTexture::setColour(Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_SetTextureColorMod(_texture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
    SDL_SetTextureBlendMode(_texture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(_texture, alpha);
}


int LTexture::getWidth()
{
    return _width;
}

int LTexture::getHeight()
{
    return _height;
}