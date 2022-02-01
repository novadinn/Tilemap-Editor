#pragma once

#include <SDL.h>
#include <map>
#include <string>
#include <memory>
#include <vector>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Rect;
struct Rectangle; // __DEBUG__

struct Graphics {
	Graphics();
	~Graphics();

	static const int kScreenWidth;
	static const int kScreenHeight;

	SDL_Surface* loadImage(const std::string& file_name, bool black_is_transparent);
	SDL_Surface* loadFont(const std::string& file_name, const std::string& text, int font_size);

	void blitSurface(SDL_Surface* source, SDL_Rect* source_rectangle, SDL_Rect* destination_rectangle, bool use_autocorrection);

	SDL_Surface* createSurface(const std::string& name, int width, int height);

	void saveSurface(SDL_Surface* surface, const std::string& file_path);

	void clear();
	void flip();
private:
	std::map<std::string, SDL_Surface*> sprite_sheets_;
	std::vector<Uint32*> pixels_;

	SDL_Window* window_;
	SDL_Surface* screen_;
};