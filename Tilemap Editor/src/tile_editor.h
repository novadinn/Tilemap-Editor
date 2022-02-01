#pragma once

#include <SDL.h>
#include <optional>
#include <memory>

#include "canvas.h"
#include "sprite.h"

struct Graphics;

struct TileEditor {
	TileEditor(Graphics& graphics);

	void moveCanvas(int x, int y, int previous_x, int previous_y);
	void scaleCanvas(float scale, int x, int y);

	void putPixel(int x, int y, Uint32 color);
	std::optional<Uint32> getPixel(int x, int y) const;

	void createCanvas(Graphics& graphics, int width, int height);
	void saveCanvas(Graphics& graphics, const std::string& file_path);
	void loadCanvas(Graphics& graphics, const std::string& file_path);

	void draw(Graphics& graphics) const;
private:
	std::unique_ptr<Canvas> canvas_;
	std::unique_ptr<Canvas> palette_;

	std::unique_ptr<Sprite> background_sprite_;
};