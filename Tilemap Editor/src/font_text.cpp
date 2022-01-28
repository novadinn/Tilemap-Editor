#include "font_text.h"

#include "graphics.h"

namespace {
	const std::string& kFontPath("prstart");
}

FontText::FontText(Graphics& graphics, const std::string& text, int font_size) {
	text_texture_ = graphics.loadFont(kFontPath, text, font_size);
	source_rectangle_.x = 0;
	source_rectangle_.y = 0;
	source_rectangle_.w = graphics.get_texture_width(text_texture_);
	source_rectangle_.h = graphics.get_texture_height(text_texture_);
}

void FontText::draw(Graphics& graphics, int x, int y) {
	SDL_Rect destination_rectangle;
	destination_rectangle.x = x;
	destination_rectangle.y = y;
	graphics.blitSurface(text_texture_, &source_rectangle_, &destination_rectangle, true);
}
