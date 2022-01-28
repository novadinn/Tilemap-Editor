#include "ui_elements.h"

namespace {
	const std::string& kButtonBackgroundSpriteFilePath("button_background");
	const int kFontSize = 16;
}


Button::Button(Graphics& graphics, const std::string& text, int x, int y) :
	text_(graphics, text, kFontSize),
	background_sprite_(graphics, kButtonBackgroundSpriteFilePath, 0, 0, text_.get_width() - 2, text_.get_height() - 2),
	collision_rectangle_(x, y, text_.get_width(), text_.get_height()),
	should_draw_background_(false) {}

TextButton::TextButton(Graphics& graphics, std::string text, int x, int y, std::function<void(Graphics& graphics)> callback) :
	Button(graphics, text, x, y),
	callback_(callback),
	active_(true) {
}

void TextButton::draw(Graphics& graphics) {
	if (!active_)
		return;
	if (should_draw_background_) {
		background_sprite_.draw(graphics, collision_rectangle_.left(), collision_rectangle_.top());
	}
	text_.draw(graphics, collision_rectangle_.left(), collision_rectangle_.top());
}

bool TextButton::interact(Graphics& graphics, int x, int y) {
	if(!active_)
		return false;
	if (collision_rectangle_.pointIntersection(x, y)) {
		callback_(graphics);
		return true;
	}
	return false;
}

void TextButton::aim(int x, int y) {
	if (!active_)
		return;
	if (collision_rectangle_.pointIntersection(x, y)) {
		should_draw_background_ = true;
	} else {
		should_draw_background_ = false;
	}
}

ButtonPopup::ButtonPopup(Graphics& graphics, const std::string& text, int x, int y, std::vector<std::shared_ptr<TextButton>> buttons) :
	Button(graphics, text, x, y),
	buttons_(buttons),
	active_(false) {
	for (size_t i = 0; i < buttons_.size(); ++i) {
		buttons_[i]->set_active(false);
	}
}

void ButtonPopup::draw(Graphics& graphics) {
	if (should_draw_background_) {
		background_sprite_.draw(graphics, collision_rectangle_.left(), collision_rectangle_.top());
	}
	text_.draw(graphics, collision_rectangle_.left(), collision_rectangle_.top());

	if (active_) {
		for (size_t i = 0; i < buttons_.size(); ++i) {
			buttons_[i]->draw(graphics);
		}
	}
}

bool ButtonPopup::interact(Graphics& graphics, int x, int y) {
	if (collision_rectangle_.pointIntersection(x, y)) {
		active_ = !active_;
		for (size_t i = 0; i < buttons_.size(); ++i) {
			buttons_[i]->set_active(active_);
		}
		return true;
	}
	return false;
}

void ButtonPopup::aim(int x, int y) {
	if (collision_rectangle_.pointIntersection(x, y)) {
		should_draw_background_ = true;
	} else {
		should_draw_background_ = false;
	}
}
