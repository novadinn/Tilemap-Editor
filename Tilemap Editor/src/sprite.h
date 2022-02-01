#pragma once

#include <SDL.h>
#include <string>

#include "rectangle.h"

struct Graphics;

struct Sprite {
	Sprite(Graphics& graphics, const std::string& file_name, int x, int y, int width, int height);

	virtual void update() {}
	void draw(Graphics& graphics, int x, int y);
protected:
	int width_, height_;
private:
	SDL_Surface* sprite_sheet_;
};