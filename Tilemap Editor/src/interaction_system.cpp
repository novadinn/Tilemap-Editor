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

	std::function<void(Graphics& graphics)> tile_create = [=](Graphics& graphics) {
		system->tile_editor_.reset(new TileEditor(graphics, 32, 32));

		std::function<void(Graphics& graphics)> pipette_mode = [=](Graphics& graphics) {
			system->mouse_state_ = PIPETTE;
		};
		std::shared_ptr<TextButton> pipette_button(new TextButton(graphics, "P", 623, 200, pipette_mode));
		system->buttons_.push_back(pipette_button);

		std::function<void(Graphics& graphics)> draw_mode = [=](Graphics& graphics) {
			system->mouse_state_ = DRAW;
		};
		std::shared_ptr<TextButton> draw_button(new TextButton(graphics, "D", 623, 180, draw_mode));
		system->buttons_.push_back(draw_button);

		std::function<void(Graphics& graphics)> choose_color = [=](Graphics& graphics) {
			std::cout << "Type in 3 numbers" << std::endl;
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
	std::shared_ptr<TextButton> tile_create_button(new TextButton(graphics, "Create Tile", 0, 16, tile_create));
	system->buttons_.push_back(tile_create_button);

	std::vector<std::shared_ptr<TextButton>> new_file_buttons;
	new_file_buttons.push_back(tile_create_button);
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
