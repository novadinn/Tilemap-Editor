#pragma once

#include <SDL.h>
#include <optional>
#include <memory>
#include <string>
#include <vector>

#include "rectangle.h"

struct SurfaceWindow;
struct Canvas;
struct Sprite;

struct Editor {
	Editor();

	void startMove(int x, int y);
	void move(int x, int y, int previous_x, int previous_y);
	void stopMove();
	void scale(float scale, int x, int y);

	void putPixel(Uint32 color, int x, int y);
	std::optional<Uint32> get_color_at_point(int x, int y);

	void createTileSheet(SurfaceWindow& graphics, int size);
	void createTileMap(SurfaceWindow& graphics, SurfaceWindow& graphics_palette, std::string& tile_sheet_name, int x_count, int y_count);
	void createPalette(SurfaceWindow& graphics);
	void saveCanvas(SurfaceWindow& graphics, const std::string& file_path);
	void loadTileSheetAsCanvas(SurfaceWindow& graphics, const std::string& file_path);
	void loadTileMap(SurfaceWindow& graphics, const std::string& file_path);
	void loadTileSheetAsPalette(SurfaceWindow& graphics, const std::string& file_path);

	void extendCanvasX(SurfaceWindow& graphics);
	void extendCanvasY(SurfaceWindow& graphics);
	void truncateCanvasX(SurfaceWindow& graphics);
	void truncateCanvasY(SurfaceWindow& graphics);

	void draw(SurfaceWindow& graphics, SurfaceWindow& graphics_palette) const;
private:
	std::shared_ptr<Canvas> canvas_;
	std::shared_ptr<Canvas> palette_;

	enum EditorMode {
		NONE,
		TILE_SHEET,
		TILE_MAP
	};
	EditorMode editor_mode_;
	int canvas_tile_size_;
	float choosed_tile_row_, choosed_tile_col_;
	std::vector<std::vector<int>> tile_grid_;
};