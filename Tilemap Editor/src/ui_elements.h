#pragma once

#include <memory>
#include <functional>

#include "font_text.h"
#include "sprite.h"
#include "rectangle.h"

struct Graphics;

struct Button {
	Button(Graphics& graphics, const std::string& text, int x, int y);
	virtual ~Button() {}

	virtual void draw(Graphics& graphics) = 0;
	virtual bool interact(Graphics& graphics, int x, int y) = 0;
	virtual void aim(int x, int y) = 0;

protected:
	FontText text_;
	Sprite background_sprite_;
	const Rectangle collision_rectangle_;

	bool should_draw_background_;
};

struct TextButton : public Button {
	TextButton(Graphics& graphics, std::string text, int x, int y, std::function<void(Graphics& graphics)> callback);

	void draw(Graphics& graphics);
	bool interact(Graphics& graphics, int x, int y);
	void aim(int x, int y);
	
	void set_active(bool toggle) { active_ = toggle; }
private:
	std::function<void(Graphics& graphics)> callback_;

	bool active_;
};

struct ButtonPopup : public Button {
	ButtonPopup(Graphics& graphics, const std::string& text, int x, int y, std::vector<std::shared_ptr<TextButton>> buttons);

	void draw(Graphics& graphics);
	bool interact(Graphics& graphics, int x, int y);
	void aim(int x, int y);
private:
	std::vector<std::shared_ptr<TextButton>> buttons_;
	bool active_;
};