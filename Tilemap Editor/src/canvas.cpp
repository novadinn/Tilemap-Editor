#include "canvas.h"

#include <memory>
#include <algorithm>
#include <vector>

#include "graphics.h"
#include "rectangle.h"

Canvas::Canvas(Graphics& graphics, float start_offset_x, float start_offset_y, int width, int height) :
	x_offset_(start_offset_x), y_offset_(start_offset_y),
	scale_x_(1), scale_y_(1),
	moving_(false),
	loaded_(false) {
	sprite_sheet_ = graphics.createSurface(width, height);
	
}

Canvas::Canvas(Graphics& graphics, const std::string& file_path, float start_offset_x, float start_offset_y) :
	x_offset_(start_offset_x), y_offset_(start_offset_y),
	scale_x_(1), scale_y_(1), 
	moving_(false),
	loaded_(true) {
	sprite_sheet_ = graphics.loadImage(file_path, false);
}

Canvas::~Canvas() {
	if(!loaded_)
		SDL_FreeSurface(sprite_sheet_);
}

void Canvas::startMoving(int x, int y) {
	moving_ = true;
}

void Canvas::move(int x, int previous_x, int y, int previous_y) {
	if (!moving_)
		return;

	x_offset_ -= ((previous_x-x) / scale_x_);
	y_offset_ -= ((previous_y-y) / scale_y_);
}

void Canvas::stopMoving() {
	moving_ = false;
}

void Canvas::scale(float scale, int x, int y) {
	float mouse_x_before_zoom, mouse_y_before_zoom;
	screenToWorld(x, y, mouse_x_before_zoom, mouse_y_before_zoom);

	scale_x_ += scale * 0.1f;
	scale_y_ += scale * 0.1f;
	scale_x_ = std::max(scale_x_, 0.1f);
	scale_y_ = std::max(scale_y_, 0.1f);

	float mouse_x_after_zoom, mouse_y_after_zoom;
	screenToWorld(x, y, mouse_x_after_zoom, mouse_y_after_zoom);

	x_offset_ -= (mouse_x_before_zoom - mouse_x_after_zoom);
	y_offset_ -= (mouse_y_before_zoom - mouse_y_after_zoom);
}

void Canvas::drawToTexture(int x, int y, Uint32 color) {
	float world_x, world_y;
	screenToWorld(x, y, world_x, world_y);
	int new_world_x = int(world_x);
	int new_world_y = int(world_y);

	if (pointSpriteIntersection(new_world_x, new_world_y)) {
		SDL_LockSurface(sprite_sheet_);
		Uint8* pixel_array = (Uint8*)sprite_sheet_->pixels_;
		pixel_array[new_world_y * sprite_sheet_->pitch + new_world_x * sprite_sheet_->format->BytesPerPixel + 0] = (color >> 16) & 0xff;
		pixel_array[new_world_y * sprite_sheet_->pitch + new_world_x * sprite_sheet_->format->BytesPerPixel + 1] = (color >> 8) & 0xff;
		pixel_array[new_world_y * sprite_sheet_->pitch + new_world_x * sprite_sheet_->format->BytesPerPixel + 2] = color & 0xff;
		SDL_UnlockSurface(sprite_sheet_);
	}
}

std::optional<Uint32> Canvas::getPixel(int x, int y) const {
	float world_x, world_y;
	screenToWorld(x, y, world_x, world_y);
	int new_world_x = int(world_x);
	int new_world_y = int(world_y);

	if (pointSpriteIntersection(new_world_x, new_world_y)) {
		Uint8* pixel_array = (Uint8*)sprite_sheet_->pixels_;
		Uint8 red = pixel_array[new_world_y * sprite_sheet_->pitch + new_world_x * sprite_sheet_->format->BytesPerPixel + 2];
		Uint8 green = pixel_array[new_world_y * sprite_sheet_->pitch + new_world_x * sprite_sheet_->format->BytesPerPixel + 1];
		Uint8 blue = pixel_array[new_world_y * sprite_sheet_->pitch + new_world_x * sprite_sheet_->format->BytesPerPixel + 0];
		return std::optional<Uint32>(SDL_MapRGB(sprite_sheet_->format, blue, green, red));
	}
	return std::nullopt;
}

void Canvas::save(Graphics& graphics, const std::string& file_path) const {
	graphics.saveSurface(sprite_sheet_, file_path);
}

void Canvas::changeSize(Graphics& graphics, int x, int y) {
	sprite_sheet_ = graphics.changeSurfaceSize(sprite_sheet_, x, y);
}

int Canvas::get_width() const {
	return sprite_sheet_->w;
}

int Canvas::get_height() const {
	return sprite_sheet_->h;
}

SDL_Surface* Canvas::get_surface() const {
	return sprite_sheet_;
}

void Canvas::draw(Graphics& graphics) const {
	SDL_Rect destination_rectangle;
	int screen_left, screen_top;
	worldToScreen(0, 0, screen_left, screen_top);
	destination_rectangle.x = screen_left;
	destination_rectangle.y = screen_top;

	int screen_right, screen_bottom;
	worldToScreen(sprite_sheet_->w * 1.0f, sprite_sheet_->h * 1.0f, screen_right, screen_bottom);
	destination_rectangle.w = screen_right - screen_left;
	destination_rectangle.h = screen_bottom - screen_top;

	// TODO: offset is ruining
	/*if (screen_left < bounds_.left()) {
		destination_rectangle.x = bounds_.left();
		destination_rectangle.w -= bounds_.left() - screen_left;
	} 
	if (screen_right > bounds_.right()) {
		destination_rectangle.w -= screen_right - bounds_.right();
	}

	if (screen_top < bounds_.top()) {
		destination_rectangle.y = bounds_.top();
		destination_rectangle.h -= bounds_.top() - screen_top;
	} 
	if (screen_bottom > bounds_.bottom()) {
		destination_rectangle.h -= screen_bottom - bounds_.bottom();
	}*/

	graphics.blitSurface(sprite_sheet_, NULL, &destination_rectangle, false);
}

void Canvas::drawGrid(Graphics& graphics, int width, int height) const {
	for (int x = 0; x < get_width(); x += width) {
		for (int y = 0; y < get_height(); y += height) {
			int screen_left, screen_top;
			worldToScreen(x * 1.0f, y * 1.0f, screen_left, screen_top);
			int screen_right, screen_bottom;
			worldToScreen(x*1.0f + width, 0, screen_right, screen_bottom);
			graphics.drawLine(screen_left, screen_top, screen_right, screen_bottom, 0);
			worldToScreen(0, y * 1.0f + height, screen_right, screen_bottom);
			graphics.drawLine(screen_left, screen_top, screen_right, screen_bottom, 0);
		}
	}
}

void Canvas::worldToScreen(float world_x, float world_y, int& screen_x, int& screen_y) const {
	screen_x = int((world_x + x_offset_) * scale_x_);
	screen_y = int((world_y + y_offset_) * scale_y_);
}

void Canvas::screenToWorld(int screen_x, int screen_y, float& world_x, float& world_y) const {
	world_x = ((float)screen_x / scale_x_) - x_offset_;
	world_y = ((float)screen_y / scale_y_) - y_offset_;
}

void Canvas::snapToBounds(const Rectangle& bounds) {
	int screen_left, screen_top;
	worldToScreen(0, 0, screen_left, screen_top);
	int screen_right, screen_bottom;
	worldToScreen(sprite_sheet_->w * 1.0f, sprite_sheet_->h * 1.0f, screen_right, screen_bottom);

	const int width = screen_right - screen_left;
	const int difference_x = std::max(width - bounds.width() / 2, 0);
	if (screen_left < bounds.left() - difference_x) {
		x_offset_ = (bounds.left() - difference_x) / scale_x_ * 1.0f;
	} else if (screen_right > bounds.right() + difference_x) {
		x_offset_ = (bounds.right() + difference_x - width) / scale_x_ * 1.0f;
	}

	const int height = screen_bottom - screen_top;
	const int difference_y = std::max(height - bounds.height() / 2, 0);
	if (screen_top < bounds.top() - difference_y) {
		y_offset_ = (bounds.top() - difference_y) / scale_y_ * 1.0f;
	} else if (screen_bottom > bounds.bottom() + difference_y) {
		y_offset_ = (bounds.bottom() + difference_y - height) / scale_y_ * 1.0f;
	}
}

bool Canvas::pointSpriteIntersection(int x, int y) const {
	return y > -1 && y < sprite_sheet_->h && x > -1 && x < sprite_sheet_->w;
}
