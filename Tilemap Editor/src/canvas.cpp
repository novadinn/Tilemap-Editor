#include "canvas.h"

#include <memory>
#include <algorithm>
#include <vector>

#include "graphics.h"
#include "rectangle.h"

Canvas::Canvas(Graphics& graphics, const std::string& file_name, float start_offset_x, float start_offset_y, int width, int height) : // file name should be unique
	sprite_sheet_(graphics.createTextureFromRenderer(file_name, width, height)),
	width_(width), height_(height),
	x_offset_(start_offset_x), y_offset_(start_offset_y),
	scale_x_(1), scale_y_(1) {
	pixels_ = new Uint32[width * height];
	memset(pixels_, 255, width * height * sizeof(Uint32));
	SDL_UpdateTexture(sprite_sheet_, NULL, pixels_, width * sizeof(Uint32));
}

Canvas::~Canvas() {
	delete[] pixels_;
}

void Canvas::move(int x_amount, int y_amount, const Rectangle& bounds) {
	x_offset_ -= (x_amount / scale_x_);
	y_offset_ -= (y_amount / scale_y_);

	snapToBounds(bounds);
}

void Canvas::scale(float scale, int x, int y, const Rectangle& bounds) {
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

	snapToBounds(bounds);
}

void Canvas::drawToTexture(int x, int y, Uint32 color) {
	float world_x, world_y;
	screenToWorld(x, y, world_x, world_y);
	int new_world_x = (int)world_x;
	int new_world_y = (int)world_y;

	if (new_world_y > -1 && new_world_y < height_ && new_world_x > -1 && new_world_x < width_) {
		if (pixels_[new_world_y * width_ + new_world_x] != color) {
			pixels_[new_world_y * width_ + new_world_x] = color;
			SDL_UpdateTexture(sprite_sheet_, NULL, pixels_, width_ * sizeof(Uint32));
		}
	}
}

std::optional<Uint32> Canvas::getPixel(int x, int y) {
	float world_x, world_y;
	screenToWorld(x, y, world_x, world_y);
	int new_world_x = (int)world_x;
	int new_world_y = (int)world_y;

	if (new_world_y > -1 && new_world_y < height_ && new_world_x > -1 && new_world_x < width_) {
		return std::optional<Uint32>(pixels_[new_world_y * width_ + new_world_x]);
	}
	return std::nullopt;
}

void Canvas::worldToScreen(float world_x, float world_y, int& screen_x, int& screen_y) {
	screen_x = (int)((world_x + x_offset_) * scale_x_);
	screen_y = (int)((world_y + y_offset_) * scale_y_);
}

void Canvas::screenToWorld(int screen_x, int screen_y, float& world_x, float& world_y) {
	world_x = ((float)screen_x / scale_x_) - x_offset_;
	world_y = ((float)screen_y / scale_y_) - y_offset_;
}

void Canvas::snapToBounds(const Rectangle& bounds) {
	int screen_left, screen_top;
	worldToScreen(0, 0, screen_left, screen_top);
	int screen_right, screen_bottom;
	worldToScreen(width_ * 1.0f, height_ * 1.0f, screen_right, screen_bottom);

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

void Canvas::draw(Graphics& graphics) {
	SDL_Rect destination_rectangle;
	int screen_left, screen_top;
	worldToScreen(0, 0, screen_left, screen_top);
	destination_rectangle.x = screen_left;
	destination_rectangle.y = screen_top;

	int screen_right, screen_bottom;
	worldToScreen(width_ * 1.0f, height_ * 1.0f, screen_right, screen_bottom);
	destination_rectangle.w = screen_right - screen_left;
	destination_rectangle.h = screen_bottom - screen_top;
	graphics.blitSurface(sprite_sheet_, NULL, &destination_rectangle, false);
}
