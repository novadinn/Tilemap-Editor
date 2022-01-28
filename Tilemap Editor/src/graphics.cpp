#include "graphics.h"

#include <SDL_ttf.h>

#include "rectangle.h"

//static
const int Graphics::kScreenWidth = 640;
//static
const int Graphics::kScreenHeight = 480;

namespace {
    const Uint8 kClearColor[] = { 51, 102, 153 };
}

Graphics::Graphics() {
    window_ = SDL_CreateWindow("Platformer Template", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, kScreenWidth, kScreenHeight, SDL_WINDOW_RESIZABLE);
    renderer_ = SDL_CreateRenderer(window_, -1, 0);
}

Graphics::~Graphics() {
    for (std::map<std::string, SDL_Texture*>::iterator iter = sprite_sheets_.begin();
        iter != sprite_sheets_.end();
        ++iter) {
        SDL_DestroyTexture(iter->second);
    }
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
}

SDL_Texture* Graphics::loadImage(const std::string& file_name, bool black_is_transparent) {
    const std::string file_path = "content/images/" + file_name + ".bmp";
    if (sprite_sheets_.count(file_path) == 0) {
        SDL_Surface* image = SDL_LoadBMP(file_path.c_str());
        if (black_is_transparent) {
            const Uint32 black_color = SDL_MapRGB(image->format, 0, 0, 0);
            SDL_SetColorKey(image, SDL_TRUE, black_color);
        }
        sprite_sheets_[file_path] = SDL_CreateTextureFromSurface(renderer_, image);
        SDL_FreeSurface(image);
    }
    return sprite_sheets_[file_path];
}

SDL_Texture* Graphics::loadFont(const std::string& file_name, const std::string& text, int font_size) {
    const std::string file_path = "content/fonts/" + file_name + ".ttf";
    const std::string index = file_path + text;
    if (sprite_sheets_.count(index) == 0) {
        TTF_Font* font = TTF_OpenFont(file_path.c_str(), font_size);
        SDL_Color color;
        color.r = 0;
        color.g = 0;
        color.b = 0;
        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
        sprite_sheets_[index] = SDL_CreateTextureFromSurface(renderer_, surface);
        SDL_FreeSurface(surface);
        TTF_CloseFont(font);
    }
    return sprite_sheets_[index];
}

void Graphics::blitSurface(SDL_Texture* source, SDL_Rect* source_rectangle, SDL_Rect* destination_rectangle, bool use_autocorrention) {
    if (use_autocorrention) {
        if (source_rectangle) {
            destination_rectangle->w = source_rectangle->w;
            destination_rectangle->h = source_rectangle->h;
        } else if (destination_rectangle) {
            uint32_t format;
            int access, w, h;
            SDL_QueryTexture(source, &format, &access, &w, &h);
            destination_rectangle->w = get_texture_width(source);
            destination_rectangle->h = get_texture_height(source);
        }
    }
    SDL_RenderCopy(renderer_, source, source_rectangle, destination_rectangle);
}

SDL_Texture* Graphics::createTextureFromRenderer(const std::string& name, int width, int height) {
    const std::string index = name + std::to_string(width) + std::to_string(height);
    if (sprite_sheets_.count(index) == 0) {
        sprite_sheets_[index] = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STREAMING, width, height);
    }
    return sprite_sheets_[index];
}

void Graphics::saveTexture(SDL_Texture* texture, const std::string& file_path) {
    SDL_Texture* rendered_texture;
    SDL_Surface* surface;
    int width;
    int height;
    int format;
    void* pixels;

    pixels = NULL;
    surface = NULL;
    rendered_texture = NULL;
    format = SDL_PIXELFORMAT_RGBA32;

    /* Get information about texture we want to save */
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);

    rendered_texture = SDL_CreateTexture(renderer_, format, SDL_TEXTUREACCESS_TARGET, width, height);

    SDL_SetRenderTarget(renderer_, rendered_texture);
    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, texture, NULL, NULL);

    pixels = malloc(width * height * SDL_BYTESPERPIXEL(format));
    SDL_RenderReadPixels(renderer_, NULL, format, pixels, width * SDL_BYTESPERPIXEL(format));
    surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, SDL_BITSPERPIXEL(format), width * SDL_BYTESPERPIXEL(format), format);
    SDL_SaveBMP(surface, file_path.c_str());
}

int Graphics::get_texture_width(SDL_Texture* source) const {
    uint32_t format;
    int access, w, h;
    SDL_QueryTexture(source, &format, &access, &w, &h);
    return w;
}

int Graphics::get_texture_height(SDL_Texture* source) const {
    uint32_t format;
    int access, w, h;
    SDL_QueryTexture(source, &format, &access, &w, &h);
    return h;
}

void Graphics::clear() {
    SDL_RenderClear(renderer_);
    SDL_SetRenderDrawColor(renderer_, kClearColor[0], kClearColor[1], kClearColor[2], SDL_ALPHA_OPAQUE);
}

void Graphics::flip() {
    SDL_RenderPresent(renderer_);
}

void Graphics::drawRect(const Rectangle& rectangle, int r, int g, int b) {
    SDL_Rect rect;
    rect.x = rectangle.left();
    rect.y = rectangle.top();
    rect.w = rectangle.width();
    rect.h = rectangle.height();

    SDL_SetRenderDrawColor(renderer_, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer_, &rect);
    SDL_SetRenderDrawColor(renderer_, kClearColor[0], kClearColor[1], kClearColor[2], SDL_ALPHA_OPAQUE);
}