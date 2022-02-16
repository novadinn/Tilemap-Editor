#pragma once

#include <SDL.h>
#include <map>
#include <string>
#include <memory>
#include <vector>

#include "rectangle.h" // __DEBUG__

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Rect;

struct Graphics {
	Graphics();
	~Graphics();

	static const int kScreenWidth;
	static const int kScreenHeight;

	SDL_Surface* loadImage(const std::string& file_name, bool black_is_transparent);
	SDL_Surface* loadFont(const std::string& file_name, const std::string& text, int font_size);
	SDL_Surface* createSurface(int width, int height);
	void saveSurface(SDL_Surface* surface, const std::string& file_path);

	SDL_Surface* changeSurfaceSize(SDL_Surface* surface, int x, int y);

	void blitSurface(SDL_Surface* source, SDL_Rect* source_rectangle, SDL_Rect* destination_rectangle, bool use_autocorrection);
	void clear();
	void flip();

	void drawRect(const Rectangle& rectangle, Uint32 border_color, Uint32 fill_color);
	void drawLine(int x1, int y1, int x2, int y2, Uint32 color);
private:
	std::map<std::string, SDL_Surface*> sprite_sheets_;
	std::vector<Uint32*> pixels_;

	SDL_Window* window_;
	SDL_Surface* screen_;
};