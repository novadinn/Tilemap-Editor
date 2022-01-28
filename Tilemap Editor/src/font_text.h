#pragma once

#include <string>
#include <SDL.h>

struct Graphics;

struct FontText {
	FontText(Graphics& graphics, const std::string& text, int font_size);

	void draw(Graphics& graphics, int x, int y);

	int get_width() const { return source_rectangle_.w; }
	int get_height() const { return source_rectangle_.h; }
private:
	SDL_Texture* text_texture_;
	SDL_Rect source_rectangle_;
};