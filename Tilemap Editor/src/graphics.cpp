#include "graphics.h"

#include <SDL_ttf.h>

//static
const int Graphics::kScreenWidth = 640;
//static
const int Graphics::kScreenHeight = 480;

namespace {
    const Uint32 kClearColor = (51 << 16) | (102 << 8) | 153;
}

Graphics::Graphics() {
    window_ = SDL_CreateWindow("Platformer Template", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, kScreenWidth, kScreenHeight, 0);
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

SDL_Surface* Graphics::createSurface(int width, int height) {
    Uint32* pixels = new Uint32[width * height];
    memset(pixels, 255, width * height * sizeof(Uint32));
    pixels_.push_back(pixels);
    return SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, 32, 4*width, SDL_PIXELFORMAT_BGRA32);
}

void Graphics::saveSurface(SDL_Surface* surface, const std::string& file_path) {
    SDL_SaveBMP(surface, file_path.c_str());
}

SDL_Surface* Graphics::changeSurfaceSize(SDL_Surface* surface, int x, int y) {
    const int width = surface->w + x;
    const int height = surface->h + y;
    Uint32* pixels = new Uint32[width * height];
    memset(pixels, 255, width * height * sizeof(Uint32));
    pixels_.push_back(pixels);
    SDL_Surface* changed_surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, 32, 4 * width, SDL_PIXELFORMAT_BGRA32);
    SDL_BlitSurface(surface, NULL, changed_surface, NULL);
    return changed_surface;
}

void Graphics::blitSurface(SDL_Surface* source, SDL_Rect* source_rectangle, SDL_Rect* destination_rectangle, bool use_autocorrention) {
    SDL_BlitScaled(source, source_rectangle, screen_, destination_rectangle);
}

void Graphics::clear() {
    SDL_FillRect(screen_, NULL, kClearColor);
}

void Graphics::flip() {
    SDL_UpdateWindowSurface(window_);
}

void Graphics::drawRect(const Rectangle& rectangle, Uint32 border_color, Uint32 fill_color) {
    SDL_Rect border_rectangle;
    border_rectangle.x = rectangle.left();
    border_rectangle.y = rectangle.top();
    border_rectangle.w = rectangle.width();
    border_rectangle.h = rectangle.height();
    SDL_FillRect(screen_, &border_rectangle, border_color);
    SDL_Rect fill_rectangle;
    fill_rectangle.x = rectangle.left() + 1;
    fill_rectangle.y = rectangle.top() + 1;
    fill_rectangle.w = rectangle.width() - 2;
    fill_rectangle.h = rectangle.height() - 2;
    SDL_FillRect(screen_, &fill_rectangle, fill_color);
}

void Graphics::drawLine(int x1, int y1, int x2, int y2, Uint32 color) {
    SDL_Rect rectangle;
    rectangle.x = x1;
    rectangle.y = y1;
    const int width = x2 - x1;
    const int height = y2 - y1;
    if (width > height) {
        rectangle.w = width;
        rectangle.h = 1;
    } else {
        rectangle.w = 1;
        rectangle.h = height;
    }
    
    SDL_FillRect(screen_, &rectangle, color);
}
