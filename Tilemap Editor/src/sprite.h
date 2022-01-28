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
	SDL_Rect source_rectangle_;
private:
	SDL_Texture* sprite_sheet_;
};