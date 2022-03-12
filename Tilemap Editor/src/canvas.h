#pragma once

#include <string>
#include <SDL.h>
#include <optional>

struct SurfaceWindow;
struct Rectangle;

struct Canvas {
	Canvas(SurfaceWindow& graphics, float start_offset_x, float start_offset_y, int width, int height);
	Canvas(SurfaceWindow& graphics, const std::string& file_path, float start_offset_x, float start_offset_y);
	~Canvas();

	void startMoving(int x, int y);
	void move(int x, int previous_x, int y, int previous_y);
	void stopMoving();
	void scale(float scale_x, int x, int y);

	void drawToTexture(int x, int y, Uint32 color);
	std::optional<Uint32> getPixel(int x, int y) const;

	void save(SurfaceWindow& graphics, const std::string& file_path) const;

	void changeSize(SurfaceWindow& graphics, int x, int y);

	int get_width() const;
	int get_height() const;
	SDL_Surface* get_surface() const;

	void draw(SurfaceWindow& graphics) const;
	void drawGrid(SurfaceWindow& graphics, int width, int height) const;

	void worldToScreen(float world_x, float world_y, int& screen_x, int& screen_y) const;
	void screenToWorld(int screen_x, int screen_y, float& world_x, float& world_y) const;

	void snapToBounds(const Rectangle& bounds);
	bool pointSpriteIntersection(int x, int y) const;
private:
	SDL_Surface* sprite_sheet_;

	float x_offset_, y_offset_;
	float scale_x_, scale_y_;

	bool moving_;
	bool loaded_;
};