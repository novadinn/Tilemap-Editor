#include "interaction_system.h"

#include <functional>
#include <iostream>

#include "tile_editor.h"
#include "ui_elements.h"
#include "input.h"
#include "graphics.h"

InteractionSystem* InteractionSystem::createSystem(Graphics& graphics) {
	InteractionSystem* system = new InteractionSystem();
	system->mouse_state_ = DRAW;
	system->current_color_ = 0;

	// Create tile editor button
	std::function<void(Graphics& graphics)> tile_editor_create = [=](Graphics& graphics) {
		system->tile_editor_.reset(new TileEditor(graphics));

		// Create tile button
		std::function<void(Graphics& graphics)> create_tile = [=](Graphics& graphics) {
			std::cout << "Type in tile size" << std::endl;
			int width;
			std::cin >> width;
			int height;
			std::cin >> height;
			system->tile_editor_->createCanvas(graphics, width, height);
			std::cout << "Finished" << std::endl;
		};
		std::shared_ptr<TextButton> create_tile_button(new TextButton(graphics, "Create tile", 96, 16, create_tile));
		system->buttons_.push_back(create_tile_button);

		// Save tile button
		std::function<void(Graphics& graphics)> save_tile = [=](Graphics& graphics) {
			std::cout << "Type in the file name" << std::endl;
			std::string name;
			std::cin >> name;
			system->tile_editor_->saveCanvas(graphics, "content/images/tiles/" + name + ".bmp");
			std::cout << "Finished" << std::endl;
		};
		std::shared_ptr<TextButton> save_tile_button(new TextButton(graphics, "Save tile", 96, 32, save_tile));
		system->buttons_.push_back(save_tile_button);

		// Load tile button
		std::function<void(Graphics& graphics)> load_tile = [=](Graphics& graphics) {
			std::cout << "Type in the file path" << std::endl;
			std::string file_path;
			std::cin >> file_path;
			system->tile_editor_->loadCanvas(graphics, "tiles/" + file_path);
			std::cout << "Finished" << std::endl;
		};
		std::shared_ptr<TextButton> load_tile_button(new TextButton(graphics, "Load tile", 96, 48, load_tile));
		system->buttons_.push_back(load_tile_button);

		// Tile.. button
		std::vector<std::shared_ptr<TextButton>> tile_buttons;
		tile_buttons.push_back(create_tile_button);
		tile_buttons.push_back(save_tile_button);
		tile_buttons.push_back(load_tile_button);
		std::shared_ptr<ButtonPopup> tile_button(new ButtonPopup(graphics, "Tile..", 96, 0, tile_buttons));
		system->buttons_.push_back(tile_button);

		// Pipette mode button
		std::function<void(Graphics& graphics)> pipette_mode = [=](Graphics& graphics) {
			system->mouse_state_ = PIPETTE;
		};
		std::shared_ptr<TextButton> pipette_button(new TextButton(graphics, "P", 623, 200, pipette_mode));
		system->buttons_.push_back(pipette_button);

		// Draw mode button
		std::function<void(Graphics& graphics)> draw_mode = [=](Graphics& graphics) {
			system->mouse_state_ = DRAW;
		};
		std::shared_ptr<TextButton> draw_button(new TextButton(graphics, "D", 623, 180, draw_mode));
		system->buttons_.push_back(draw_button);

		// Choose color button
		std::function<void(Graphics& graphics)> choose_color = [=](Graphics& graphics) {
			std::cout << "Type in 3 RGB values" << std::endl;
			int red;
			std::cin >> red;
			int green;
			std::cin >> green;
			int blue;
			std::cin >> blue;
			system->current_color_ = (red << 16) | (green << 8) | blue;
			std::cout << "Finished" << std::endl;
		};
		std::shared_ptr<TextButton> choose_color_button(new TextButton(graphics, "C", 623, 160, choose_color));
		system->buttons_.push_back(choose_color_button);
	};
	std::shared_ptr<TextButton> tile_editor_create_button(new TextButton(graphics, "Create Tile Editor", 0, 16, tile_editor_create));
	system->buttons_.push_back(tile_editor_create_button);

	// File.. button
	std::vector<std::shared_ptr<TextButton>> new_file_buttons;
	new_file_buttons.push_back(tile_editor_create_button);
	std::shared_ptr<ButtonPopup> new_file(new ButtonPopup(graphics, "File..", 0, 0, new_file_buttons));
	system->buttons_.push_back(new_file);

	return system;
}

void InteractionSystem::checkForInteractions(Graphics& graphics, Input& input, int x, int y, int previous_x, int previous_y) {
	for (size_t i = 0; i < buttons_.size(); ++i) {
		buttons_[i]->aim(x, y);
		if (input.wasMouseButtonPressed(SDL_BUTTON_LEFT)) {
			if (buttons_[i]->interact(graphics, x, y)) {
				break;
			}
		}
	}

	if (tile_editor_) {
		if (input.isMouseButtonPressed(SDL_BUTTON_MIDDLE)) {
			tile_editor_->moveCanvas(x, y, previous_x, previous_y);
		}
		if (input.isMouseButtonPressed(SDL_BUTTON_LEFT)) {
			if (mouse_state_ == DRAW)
				tile_editor_->putPixel(x, y, current_color_);
			else if (mouse_state_ == PIPETTE) {
				if (tile_editor_->getPixel(x, y) != std::nullopt) {
					current_color_ = *tile_editor_->getPixel(x, y);
				}
			}
		}

		if (input.getWheelMovement() != 0) {
			tile_editor_->scaleCanvas(input.getWheelMovement(), x, y);
		}
	}
}

void InteractionSystem::draw(Graphics& graphics) const {
	if (tile_editor_)
		tile_editor_->draw(graphics);

	for (size_t i = 0; i < buttons_.size(); ++i) {
		buttons_[i]->draw(graphics);
	}
}
