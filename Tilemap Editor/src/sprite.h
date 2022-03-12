#pragma once

#include <SDL.h>
#include <string>

struct SurfaceWindow;

struct Sprite {
	Sprite(SurfaceWindow& graphics, const std::string& file_name, int x, int y, int width, int height);

	virtual void update() {}
	void draw(SurfaceWindow& graphics, int x, int y);
protected:
	int width_, height_;
private:
	SDL_Surface* sprite_sheet_;
};