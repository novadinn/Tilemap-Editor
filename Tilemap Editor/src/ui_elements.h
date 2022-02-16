#pragma once

#include <memory>
#include <set>
#include <functional>
#include <optional>
#include <SDL.h>

#include "rectangle.h"

struct Graphics;

struct Sprite {
	Sprite(Graphics& graphics, const std::string& file_name, int x, int y, int width, int height);

	virtual void update() {}
	void draw(Graphics& graphics, int x, int y);
protected:
	int width_, height_;
private:
	SDL_Surface* sprite_sheet_;
};

struct FontText {
	FontText(Graphics& graphics, const std::string& text, int font_size);

	void draw(Graphics& graphics, int x, int y);

	int get_width() const { return text_surface_->w; }
	int get_height() const { return text_surface_->h; }
private:
	SDL_Surface* text_surface_;
};

struct Button {
	Button(Graphics& graphics, const std::string& text, int x, int y);
	Button(Graphics& graphics, const Rectangle& rectangle);
	virtual ~Button() { buttons_.erase(this); }

	virtual void draw(Graphics& graphics) = 0;
	virtual bool interact(Graphics& graphics, int x, int y) = 0;
	virtual void aim(int x, int y) = 0;

	static void updateAll(int x, int y);
	static void drawAll(Graphics& graphics);
	static void onClickEvent(Graphics& graphics, int x, int y);

protected:
	Rectangle collision_rectangle_;

private:
	static std::set<Button*> buttons_;
};

struct TextButton : public Button {
	TextButton(Graphics& graphics, std::string text, int x, int y, std::function<void(Graphics& graphics)> callback);

	void draw(Graphics& graphics);
	bool interact(Graphics& graphics, int x, int y);
	void aim(int x, int y);
	
	void set_active(bool toggle) { active_ = toggle; }
private:
	std::shared_ptr<FontText> text_;
	std::shared_ptr<Sprite> background_sprite_;

	std::function<void(Graphics& graphics)> callback_;

	bool active_;
	bool should_draw_background_;
};

struct ButtonPopup : public Button {
	ButtonPopup(Graphics& graphics, const std::string& text, int x, int y, std::vector<std::shared_ptr<TextButton>> buttons);

	void draw(Graphics& graphics);
	bool interact(Graphics& graphics, int x, int y);
	void aim(int x, int y);

	void set_active(bool toggle) { active_ = toggle; }
private:
	std::shared_ptr<FontText> text_;
	std::shared_ptr<Sprite> background_sprite_;

	std::vector<std::shared_ptr<TextButton>> buttons_;

	bool active_;
	bool should_draw_background_;
};