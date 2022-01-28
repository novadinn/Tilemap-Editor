#include "sprite.h"

#include "graphics.h"

#include <iostream>

Sprite::Sprite(Graphics& graphics, const std::string& file_name, int x, int y, int width, int height) {
	bool black_is_transparent = true;
	sprite_sheet_ = graphics.loadImage(file_name, black_is_transparent);
	source_rectangle_.x = x;
	source_rectangle_.y = y;
	source_rectangle_.w = width;
	source_rectangle_.h = height;
}

void Sprite::draw(Graphics& graphics, int x, int y) {
	SDL_Rect destination_rectangle;
	destination_rectangle.x = x;
	destination_rectangle.y = y;
	graphics.blitSurface(sprite_sheet_, &source_rectangle_, &destination_rectangle, true);
}