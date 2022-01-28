#pragma once

#include <SDL.h>
#include <map>
#include <string>
#include <memory>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Rect;
struct Rectangle; // __DEBUG__

struct Graphics {
	Graphics();
	~Graphics();

	static const int kScreenWidth;
	static const int kScreenHeight;

	SDL_Texture* loadImage(const std::string& file_name, bool black_is_transparent);
	SDL_Texture* loadFont(const std::string& file_name, const std::string& text, int font_size);

	void blitSurface(SDL_Texture* source, SDL_Rect* source_rectangle, SDL_Rect* destination_rectangle, bool use_autocorrection);

	SDL_Texture* createTextureFromRenderer(const std::string& name, int width, int height);

	void saveTexture(SDL_Texture* texture, const std::string& file_path);

	int get_texture_width(SDL_Texture* source) const;
	int get_texture_height(SDL_Texture* source) const;

	void clear();
	void flip();

	void drawRect(const Rectangle& rectangle, int r, int g, int b);
private:
	

	std::map<std::string, SDL_Texture*> sprite_sheets_;

	SDL_Window* window_;
	SDL_Renderer* renderer_;
};