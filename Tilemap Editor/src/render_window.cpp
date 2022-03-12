#include "render_window.h"

#include "rectangle.h"

namespace {
	const int kScreenWidth = 640;
	const int kScreenHeight = 480;
}

RenderWindow::RenderWindow() {
	window_ = SDL_CreateWindow("Tilemap editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, kScreenWidth, kScreenHeight, 0);
	renderer_ = SDL_CreateRenderer(window_, -1, 0);
}

RenderWindow::~RenderWindow() {
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
}

void RenderWindow::clear() {
	SDL_RenderClear(renderer_);
}

void RenderWindow::flip() {
	SDL_RenderPresent(renderer_);
}