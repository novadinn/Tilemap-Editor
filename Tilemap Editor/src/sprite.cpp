#include "sprite.h"

#include "surface_window.h"


Sprite::Sprite(SurfaceWindow& graphics, const std::string& file_name, int x, int y, int width, int height) :
	width_(width), height_(height) {
	bool black_is_transparent = true;
	sprite_sheet_ = graphics.loadImage(file_name, black_is_transparent);
}

void Sprite::draw(SurfaceWindow& graphics, int x, int y) {
	SDL_Rect destination_rectangle;
	destination_rectangle.x = x;
	destination_rectangle.y = y;
	destination_rectangle.w = width_;
	destination_rectangle.h = height_;
	graphics.blitSurface(sprite_sheet_, NULL, &destination_rectangle, true);
}