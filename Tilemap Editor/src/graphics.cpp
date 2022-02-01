#include "graphics.h"

#include <SDL_ttf.h>

#include "rectangle.h"

//static
const int Graphics::kScreenWidth = 640;
//static
const int Graphics::kScreenHeight = 480;

namespace {
    const Uint32 kClearColor = (51 << 16) | (102 << 8) | 153;
}

Graphics::Graphics() {
    window_ = SDL_CreateWindow("Platformer Template", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, kScreenWidth, kScreenHeight, SDL_WINDOW_RESIZABLE);
    screen_ = SDL_GetWindowSurface(window_);
}

Graphics::~Graphics() {
    for (std::map<std::string, SDL_Surface*>::iterator iter = sprite_sheets_.begin();
        iter != sprite_sheets_.end();
        ++iter) {
        SDL_FreeSurface(iter->second);
    }
    for (size_t i = 0; i < pixels_.size(); ++i) {
        delete[] pixels_[i];
    }
    SDL_FreeSurface(screen_);
    SDL_DestroyWindow(window_);
}

#include <iostream>

SDL_Surface* Graphics::loadImage(const std::string& file_name, bool black_is_transparent) {
    const std::string file_path = "content/images/" + file_name + ".bmp";
    if (sprite_sheets_.count(file_path) == 0) {
        SDL_Surface* image = SDL_LoadBMP(file_path.c_str());
        if (black_is_transparent) {
            const Uint32 black_color = SDL_MapRGB(image->format, 0, 0, 0);
            SDL_SetColorKey(image, SDL_TRUE, black_color);
        }
        sprite_sheets_[file_path] = image;
    }
    return sprite_sheets_[file_path];
}

SDL_Surface* Graphics::loadFont(const std::string& file_name, const std::string& text, int font_size) {
    const std::string file_path = "content/fonts/" + file_name + ".ttf";
    const std::string index = file_path + text;
    if (sprite_sheets_.count(index) == 0) {
        TTF_Font* font = TTF_OpenFont(file_path.c_str(), font_size);
        SDL_Color color;
        color.r = 0;
        color.g = 0;
        color.b = 0;
        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
        sprite_sheets_[index] = surface;
        TTF_CloseFont(font);
    }
    return sprite_sheets_[index];
}

void Graphics::blitSurface(SDL_Surface* source, SDL_Rect* source_rectangle, SDL_Rect* destination_rectangle, bool use_autocorrention) {
    SDL_BlitScaled(source, NULL, screen_, destination_rectangle);
}

SDL_Surface* Graphics::createSurface(const std::string& name, int width, int height) {
    if (sprite_sheets_.count(name) == 0) {
        Uint32* pixels = new Uint32[width * height];
        memset(pixels, 255, width * height * sizeof(Uint32));
        pixels_.push_back(pixels);
        sprite_sheets_[name] = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, 32, 4*width, SDL_PIXELFORMAT_RGBA32);
    }
    return sprite_sheets_[name];
}

void Graphics::saveSurface(SDL_Surface* surface, const std::string& file_path) {
    SDL_SaveBMP(surface, file_path.c_str());
}

void Graphics::clear() {
    SDL_FillRect(screen_, NULL, kClearColor);
}

void Graphics::flip() {
    SDL_UpdateWindowSurface(window_);
}