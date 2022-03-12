#pragma once

#include <vector>
#include <memory>
#include <SDL.h>

struct SurfaceWindow;
struct Input;
struct Editor;

struct Application {
	Application();
	~Application();
private:
	void eventLoop();

	std::shared_ptr<Editor> editor_;
	Uint32 current_color_;
};