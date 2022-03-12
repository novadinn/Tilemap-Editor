#pragma once

#include <SDL.h>

struct RenderWindow {
	RenderWindow();
	~RenderWindow();

	void clear();
	void flip();

	SDL_Window* get_window() const { return window_; }
	SDL_Renderer* get_renderer() const { return renderer_; }

	Uint32 get_window_id() const { return SDL_GetWindowID(window_); }
private:
	SDL_Window* window_;
	SDL_Renderer* renderer_;
};