#pragma once

#include <vector>
#include <memory>

#include "ui_elements.h"

struct Graphics;
struct Input;
struct Button;
struct Editor;

struct Application {
	Application();
	~Application();
private:
	void eventLoop();

	void draw(Graphics& graphics);

	enum MouseState {
		DRAW,
		PIPETTE
	};
	MouseState mouse_state_;

	std::shared_ptr<Editor> editor_;
	std::vector<std::shared_ptr<Button>> buttons_;
};