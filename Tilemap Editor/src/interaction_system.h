#pragma once

#include <SDL.h>
#include <vector>
#include <memory>

#include "sprite.h"

struct Graphics;
struct Input;
struct Button;
struct TileEditor;

struct InteractionSystem {
	static InteractionSystem* createSystem(Graphics& graphics);

	void checkForInteractions(Graphics& graphisc, Input& input, int x, int y, int previous_x, int previous_y);

	void draw(Graphics& graphics) const;
private:
	enum MouseState {
		DRAW,
		PIPETTE
	};
	MouseState mouse_state_;
	Uint32 current_color_;

	std::vector<std::shared_ptr<Button>> buttons_;

	std::shared_ptr<TileEditor> tile_editor_;
};