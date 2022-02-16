#include "ui_elements.h"

#include "graphics.h"
#include "rectangle.h"

//static
std::set<Button*> Button::buttons_;

namespace {
	const std::string& kFontPath("prstart");

	const std::string& kButtonBackgroundSpriteFilePath("button_background");
	const int kFontSize = 16;
}

Sprite::Sprite(Graphics& graphics, const std::string& file_name, int x, int y, int width, int height) :
	width_(width), height_(height) {
	bool black_is_transparent = true;
	sprite_sheet_ = graphics.loadImage(file_name, black_is_transparent);
}

void Sprite::draw(Graphics& graphics, int x, int y) {
	SDL_Rect destination_rectangle;
	destination_rectangle.x = x;
	destination_rectangle.y = y;
	destination_rectangle.w = width_;
	destination_rectangle.h = height_;
	graphics.blitSurface(sprite_sheet_, NULL, &destination_rectangle, true);
}

FontText::FontText(Graphics& graphics, const std::string& text, int font_size) {
	text_surface_ = graphics.loadFont(kFontPath, text, font_size);
}

void FontText::draw(Graphics& graphics, int x, int y) {
	SDL_Rect destination_rectangle;
	destination_rectangle.x = x;
	destination_rectangle.y = y;
	destination_rectangle.w = text_surface_->w;
	destination_rectangle.h = text_surface_->h;
	graphics.blitSurface(text_surface_, NULL, &destination_rectangle, true);
}

Button::Button(Graphics& graphics, const std::string& text, int x, int y) {
	FontText font_text(graphics, text, kFontSize);
	collision_rectangle_ = Rectangle(x, y, font_text.get_width(), font_text.get_height());
	buttons_.insert(this);
}

Button::Button(Graphics& graphics, const Rectangle& rectangle) :
	collision_rectangle_(rectangle) {
	buttons_.insert(this);
}

void Button::updateAll(int x, int y) {
	for (std::set<Button*>::iterator iter = buttons_.begin();
		iter != buttons_.end();
		++iter) {
		(*iter)->aim(x, y);
	}
}

void Button::drawAll(Graphics& graphics) {
	for (std::set<Button*>::iterator iter = buttons_.begin();
		iter != buttons_.end();
		++iter) {
		(*iter)->draw(graphics);
	}
}

void Button::onClickEvent(Graphics& graphics, int x, int y) {
	for (std::set<Button*>::iterator iter = buttons_.begin();
		iter != buttons_.end();
		++iter) {
		if ((*iter)->interact(graphics, x, y))
			break;
	}
}

TextButton::TextButton(Graphics& graphics, std::string text, int x, int y, std::function<void(Graphics& graphics)> callback) :
	Button(graphics, text, x, y),
	text_(std::make_shared<FontText>(graphics, text, kFontSize)),
	callback_(callback),
	active_(true),
	should_draw_background_(false) {
	background_sprite_ = std::make_shared<Sprite>(graphics, kButtonBackgroundSpriteFilePath, 0, 0, text_->get_width() - 2, text_->get_height() - 2);
}

void TextButton::draw(Graphics& graphics) {
	if (!active_)
		return;
	if (should_draw_background_) {
		background_sprite_->draw(graphics, collision_rectangle_.left(), collision_rectangle_.top());
	}
	text_->draw(graphics, collision_rectangle_.left(), collision_rectangle_.top());
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
	text_(std::make_shared<FontText>(graphics, text, kFontSize)),
	buttons_(buttons),
	active_(false),
	should_draw_background_(false) {
	background_sprite_ = std::make_shared<Sprite>(graphics, kButtonBackgroundSpriteFilePath, 0, 0, text_->get_width() - 2, text_->get_height() - 2);

	for (size_t i = 0; i < buttons_.size(); ++i) {
		buttons_[i]->set_active(false);
	}
}

void ButtonPopup::draw(Graphics& graphics) {
	if (should_draw_background_) {
		background_sprite_->draw(graphics, collision_rectangle_.left(), collision_rectangle_.top());
	}
	text_->draw(graphics, collision_rectangle_.left(), collision_rectangle_.top());
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