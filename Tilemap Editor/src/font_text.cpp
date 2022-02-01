#include "font_text.h"

#include "graphics.h"

namespace {
	const std::string& kFontPath("prstart");
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
