#pragma once

#include <SDL.h>
#include <optional>
#include <memory>
#include <string>
#include <vector>

#include "rectangle.h"

struct Graphics;
struct Canvas;
struct Sprite;

struct Editor {
	Editor(Graphics& graphics);

	void startMove(int x, int y);
	void move(int x, int y, int previous_x, int previous_y);
	void stopMove();
	void scale(float scale, int x, int y);

	void putPixel(int x, int y);
	void set_color(Uint32 color);
	void setColorAtPoint(int x, int y);

	void createTileSheet(Graphics& graphics, int size);
	void createTileMap(Graphics& graphics, std::string& tile_sheet_name, int x_count, int y_count);
	void createPalette(Graphics& graphics);
	void saveCanvas(Graphics& graphics, const std::string& file_path);
	void loadTileSheetAsCanvas(Graphics& graphics, const std::string& file_path);
	void loadTileMap(Graphics& graphics, const std::string& file_path);
	void loadTileSheetAsPalette(Graphics& graphics, const std::string& file_path);

	void extendCanvasX(Graphics& graphics);
	void extendCanvasY(Graphics& graphics);
	void truncateCanvasX(Graphics& graphics);
	void truncateCanvasY(Graphics& graphics);

	void draw(Graphics& graphics) const;
private:
	std::shared_ptr<Canvas> canvas_;
	std::shared_ptr<Canvas> palette_;

	Uint32 current_color_;

	enum EditorMode {
		NONE,
		TILE_SHEET,
		TILE_MAP
	};
	EditorMode editor_mode_;
	int canvas_tile_size_;
	float choosed_tile_row_, choosed_tile_col_;
	std::vector<std::vector<int>> tile_grid_;

	std::shared_ptr<Sprite> background_sprite_;
	std::shared_ptr<Sprite> palette_sprite_;
};