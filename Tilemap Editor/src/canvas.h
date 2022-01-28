#pragma once

#include <string>
#include <SDL.h>
#include <optional>

struct Graphics;
struct Rectangle;

struct Canvas {
	Canvas(Graphics& graphics, const std::string& file_name, float start_offset_x, float start_offset_y, int width, int height);
	~Canvas();

	void move(int x_amount, int y_amount, const Rectangle& bounds);
	void scale(float scale_x, int x, int y, const Rectangle& bounds);

	void drawToTexture(int x, int y, Uint32 color);
	std::optional<Uint32> getPixel(int x, int y);

	void worldToScreen(float world_x, float world_y, int& screen_x, int& screen_y);
	void screenToWorld(int screen_x, int screen_y, float& world_x, float& world_y);

	void snapToBounds(const Rectangle& bounds);

	void draw(Graphics& graphics);
private:
	SDL_Texture* sprite_sheet_;
	Uint32* pixels_;

	int width_, height_;
	float x_offset_, y_offset_;
	float scale_x_, scale_y_;
};