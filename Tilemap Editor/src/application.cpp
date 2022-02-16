#include "application.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <functional>
#include <iostream>

#include "editor.h"
#include "ui_elements.h"
#include "input.h"
#include "graphics.h"

namespace {
	const int kDeltaTime = 1;
	const int kGameSpeed = 1;

	const int kScrollSpeed = 2;
}

Application::Application() {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	eventLoop();
}

Application::~Application() {
	SDL_Quit();
	TTF_Quit();
}

void Application::eventLoop() {
	Graphics graphics;
	Input input;
	SDL_Event event;

	mouse_state_ = DRAW;

	editor_.reset(new Editor(graphics));

	const int create_tile_sheet_button_x = 0;

	// Create as tile sheet button
	std::function<void(Graphics& graphics)> create_as_tile_sheet = [=](Graphics& graphics) {
		std::cout << "Type in tile size, or \";c\" for cancel" << std::endl;
		std::string size;
		std::cin >> size;
		if (size == ";c") {
			std::cout << "Canceled" << std::endl;
			return;
		}
		editor_->createTileSheet(graphics, std::max(stoi(size), 1));
		std::cout << "Finished" << std::endl;
	};
	std::shared_ptr<TextButton> create_as_tile_sheet_button(new TextButton(graphics, "Create as tile sheet", create_tile_sheet_button_x, 16, create_as_tile_sheet));

	// Create as tile map button
	std::function<void(Graphics& graphics)> create_as_tile_map = [=](Graphics& graphics) {
		std::cout << "Type in tile sheet file name, or \";c\" for cancel" << std::endl;
		std::string tile_sheet;
		std::cin >> tile_sheet;
		if (tile_sheet == ";c") {
			std::cout << "Canceled" << std::endl;
			return;
		}
		std::cout << "Type in x and y tile count" << std::endl;
		int x;
		std::cin >> x;
		int y;
		std::cin >> y;
		editor_->createTileMap(graphics, tile_sheet, std::max(x, 1), std::max(y, 1));
		std::cout << "Finished" << std::endl;
	};
	std::shared_ptr<TextButton> create_as_tile_map_button(new TextButton(graphics, "Create as tile map", create_tile_sheet_button_x, 32, create_as_tile_map));

	std::function<void(Graphics& graphics)> create_palette = [=](Graphics& graphics) {
		editor_->createPalette(graphics);
	};
	std::shared_ptr<TextButton> create_palette_button(new TextButton(graphics, "Create palette", create_tile_sheet_button_x, 48, create_palette));

	// Extend tile sheet x button
	std::function<void(Graphics& graphics)> extend_canvas_x = [=](Graphics& graphics) {
		editor_->extendCanvasX(graphics);
	};
	std::shared_ptr<TextButton> extend_canvas_x_button(new TextButton(graphics, "Extend X", create_tile_sheet_button_x, 64, extend_canvas_x));

	// Extend tile sheet y button
	std::function<void(Graphics& graphics)> extend_canvas_y = [=](Graphics& graphics) {
		editor_->extendCanvasY(graphics);
	};
	std::shared_ptr<TextButton> extend_canvas_y_button(new TextButton(graphics, "Extend Y", create_tile_sheet_button_x, 80, extend_canvas_y));

	// Truncate tile sheet x button
	std::function<void(Graphics& graphics)> truncate_canvas_x = [=](Graphics& graphics) {
		editor_->truncateCanvasX(graphics);
	};
	std::shared_ptr<TextButton> truncate_canvas_x_button(new TextButton(graphics, "Truncate X", create_tile_sheet_button_x, 96, truncate_canvas_x));

	// Truncate tile sheet y button
	std::function<void(Graphics& graphics)> truncate_canvas_y = [=](Graphics& graphics) {
		editor_->truncateCanvasY(graphics);
	};
	std::shared_ptr<TextButton> truncate_canvas_y_button(new TextButton(graphics, "Truncate Y", create_tile_sheet_button_x, 112, truncate_canvas_y));

	// Save canvas button
	std::function<void(Graphics& graphics)> save_canvas = [=](Graphics& graphics) {
		std::cout << "Type in file name" << std::endl;
		std::string name;
		std::cin >> name;
		editor_->saveCanvas(graphics, "content/images/tiles/" + name);
		std::cout << "Finished" << std::endl;
	};
	std::shared_ptr<TextButton> save_canvas_button(new TextButton(graphics, "Save file", 498, 0, save_canvas));
	buttons_.push_back(save_canvas_button);

	// Load button
	const int load_buttons_x = 150;
	std::function<void(Graphics& graphics)> load_tile_sheet_as_canvas = [=](Graphics& graphics) {
		std::cout << "Type in the file path, or \";c\" for cancel" << std::endl;
		std::string file_path;
		std::cin >> file_path;
		if (file_path == ";c") {
			std::cout << "Canceled" << std::endl;
			return;
		}
		editor_->loadTileSheetAsCanvas(graphics, "tiles/" + file_path);
		std::cout << "Finished" << std::endl;
	};
	std::shared_ptr<TextButton> load_tile_sheet_as_canvas_button(new TextButton(graphics, "Load tile sheet as canvas", load_buttons_x, 16, load_tile_sheet_as_canvas));

	std::function<void(Graphics& graphics)> load_tile_sheet_as_palette = [=](Graphics& graphics) {
		std::cout << "Type in the file path, or \";c\" for cancel" << std::endl;
		std::string file_path;
		std::cin >> file_path;
		if (file_path == ";c") {
			std::cout << "Canceled" << std::endl;
			return;
		}
		editor_->loadTileSheetAsPalette(graphics, "tiles/" + file_path);
		std::cout << "Finished" << std::endl;
	};
	std::shared_ptr<TextButton> load_tile_sheet_as_palette_button(new TextButton(graphics, "Load tile sheet as palette", load_buttons_x, 32, load_tile_sheet_as_palette));

	std::function <void(Graphics& graphics)> load_tile_map = [=](Graphics& graphics) {
		std::cout << "Type in the file path, or \";c\" for cancel" << std::endl;
		std::string file_path;
		std::cin >> file_path;
		if (file_path == ";c") {
			std::cout << "Canceled" << std::endl;
			return;
		}
		editor_->loadTileMap(graphics, "tiles/" + file_path);
		std::cout << "Finished" << std::endl;
	};
	std::shared_ptr<TextButton> load_tile_map_button(new TextButton(graphics, "Load tile map", load_buttons_x, 48, load_tile_map));

	std::vector<std::shared_ptr<TextButton>> load_buttons;
	load_buttons.push_back(load_tile_sheet_as_canvas_button);
	load_buttons.push_back(load_tile_sheet_as_palette_button);
	load_buttons.push_back(load_tile_map_button);
	std::shared_ptr<ButtonPopup> load_button(new ButtonPopup(graphics, "Load..", load_buttons_x, 0, load_buttons));
	buttons_.push_back(load_button);

	// Canvas.. button
	std::vector<std::shared_ptr<TextButton>> canvas_buttons;
	canvas_buttons.push_back(create_as_tile_sheet_button);
	canvas_buttons.push_back(create_as_tile_map_button);
	canvas_buttons.push_back(create_palette_button);
	canvas_buttons.push_back(extend_canvas_x_button);
	canvas_buttons.push_back(extend_canvas_y_button);
	canvas_buttons.push_back(truncate_canvas_x_button);
	canvas_buttons.push_back(truncate_canvas_y_button);
	std::shared_ptr<ButtonPopup> tile_button(new ButtonPopup(graphics, "Canvas..", create_tile_sheet_button_x, 0, canvas_buttons));
	buttons_.push_back(tile_button);

	// Pipette mode button
	std::function<void(Graphics& graphics)> pipette_mode = [=](Graphics& graphics) {
		mouse_state_ = PIPETTE;
	};
	std::shared_ptr<TextButton> pipette_button(new TextButton(graphics, "P", 623, 200, pipette_mode));
	buttons_.push_back(pipette_button);

	// Draw mode button
	std::function<void(Graphics& graphics)> draw_mode = [=](Graphics& graphics) {
		mouse_state_ = DRAW;
	};
	std::shared_ptr<TextButton> draw_button(new TextButton(graphics, "D", 623, 180, draw_mode));
	buttons_.push_back(draw_button);

	// Choose color button
	std::function<void(Graphics& graphics)> choose_color = [=](Graphics& graphics) {
		std::cout << "Type in RGB values" << std::endl;
		unsigned int red;
		std::cin >> red;
		unsigned int green;
		std::cin >> green;
		unsigned int blue;
		std::cin >> blue;
		editor_->set_color((blue << 16) | (green << 8) | red);
		std::cout << "Finished" << std::endl;
	};
	std::shared_ptr<TextButton> choose_color_button(new TextButton(graphics, "C", 623, 160, choose_color));
	buttons_.push_back(choose_color_button);

	int last_update_time = SDL_GetTicks();
	int frame_time_accumulator = 0;
	bool running = true;
	while (running) {
		const int start_frame_time = SDL_GetTicks();
		const int frame_time = start_frame_time - last_update_time;
		last_update_time = start_frame_time;
		frame_time_accumulator += frame_time;

		while (frame_time_accumulator >= kDeltaTime) {
			input.beginNewFrame();
			int previous_mouse_x, previous_mouse_y;
			SDL_GetMouseState(&previous_mouse_x, &previous_mouse_y);

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (!event.key.repeat)
						input.keyDownEvent(event);
					break;
				case SDL_KEYUP:
					input.keyUpEvent(event);
					break;
				case SDL_MOUSEBUTTONDOWN:
					input.mouseDownEvent(event);
					break;
				case SDL_MOUSEBUTTONUP:
					input.mouseUpEvent(event);
					break;
				case SDL_MOUSEWHEEL:
					input.mouseWheelEvent(event);
					break;
				default:
					break;
				}
			}

			if (input.wasKeyPressed(SDLK_ESCAPE)) {
				running = false;
			}

			int mouse_x, mouse_y;
			SDL_GetMouseState(&mouse_x, &mouse_y);

			Button::updateAll(mouse_x, mouse_y);
			if (input.wasMouseButtonPressed(SDL_BUTTON_LEFT)) {
				Button::onClickEvent(graphics, mouse_x, mouse_y);
			}

			if (editor_) {
				if (input.wasMouseButtonPressed(SDL_BUTTON_MIDDLE)) {
					editor_->startMove(mouse_x, mouse_y);
				} else if (input.isMouseButtonPressed(SDL_BUTTON_MIDDLE)) {
					editor_->move(mouse_x, mouse_y, previous_mouse_x, previous_mouse_y);
				} else if (input.wasMouseButtonReleased(SDL_BUTTON_MIDDLE)) {
					editor_->stopMove();
				}

				if (input.isMouseButtonPressed(SDL_BUTTON_LEFT)) {
					if (mouse_state_ == DRAW)
						editor_->putPixel(mouse_x, mouse_y);
					else if (mouse_state_ == PIPETTE) {
						editor_->setColorAtPoint(mouse_x, mouse_y);
					}
				}

				if (input.getWheelMovement() != 0) {
					editor_->scale(input.getWheelMovement() * kScrollSpeed, mouse_x, mouse_y);
				}
			}

			frame_time_accumulator -= kDeltaTime;
		}

		draw(graphics);
	}
}

void Application::draw(Graphics& graphics) {
	graphics.clear();

	if (editor_)
		editor_->draw(graphics);

	Button::drawAll(graphics);

	graphics.flip();
}
