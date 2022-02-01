#pragma once

#include <string>
#include <SDL.h>

struct Graphics;

struct FontText {
	FontText(Graphics& graphics, const std::string& text, int font_size);

	void draw(Graphics& graphics, int x, int y);

	int get_width() const { return text_surface_->w; }
	int get_height() const { return text_surface_->h; }
private:
	SDL_Surface* text_surface_;
};