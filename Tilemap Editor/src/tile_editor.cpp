#include "tile_editor.h"

#include "rectangle.h"
#include "graphics.h"

namespace {
	const Rectangle kBackgroundBoundsRectangle(68, 40, 553, 401);

	const float kPaletteX = 2;
	const float kPaletteY = 14;
	const int kPaletteWidth = 16;
	const int kPaletteHeight = 112;
	const int kPaletteScale = 24;
}

TileEditor::TileEditor(Graphics& graphics) :
	palette_(std::make_unique<Canvas>(graphics, "Palette", kPaletteX, kPaletteY, kPaletteWidth, kPaletteHeight)),
	background_sprite_(std::make_unique<Sprite>(graphics, "background", 0, 0, Graphics::kScreenWidth, Graphics::kScreenHeight)) {
	palette_->scale(kPaletteScale, 0, 0, Rectangle(0,0,1000,1000));
}

void TileEditor::moveCanvas(int x, int y, int previous_x, int previous_y) {
	if(canvas_)
		canvas_->move(previous_x - x, previous_y - y, kBackgroundBoundsRectangle);
}

void TileEditor::scaleCanvas(float scale, int x, int y) {
	if(canvas_)
		canvas_->scale(scale, x, y, kBackgroundBoundsRectangle);
}

void TileEditor::putPixel(int x, int y, Uint32 color) {
	if(canvas_)
		canvas_->drawToTexture(x, y, color);
	palette_->drawToTexture(x, y, color);
}

std::optional<Uint32> TileEditor::getPixel(int x, int y) const {
	if (canvas_ && canvas_->getPixel(x, y) != std::nullopt) {
		return *canvas_->getPixel(x, y);
	} else if (palette_->getPixel(x, y) != std::nullopt) {
		return *palette_->getPixel(x, y);
	}
	return std::nullopt;
}

void TileEditor::createCanvas(Graphics& graphics, int width, int height) {
	canvas_.reset(new Canvas(graphics, "Canvas", Graphics::kScreenWidth * 1.0f / 2, Graphics::kScreenHeight * 1.0f / 2, width, height));
}

void TileEditor::saveCanvas(Graphics& graphics, const std::string& file_path) {
	if(canvas_)
		canvas_->save(graphics, file_path);
}

void TileEditor::loadCanvas(Graphics& graphics, const std::string& file_path) {
	canvas_.reset(new Canvas(graphics, file_path, Graphics::kScreenWidth * 1.0f/2, Graphics::kScreenHeight * 1.0f/2));
}

void TileEditor::draw(Graphics& graphics) const {
	if (canvas_) 
		canvas_->draw(graphics);
	background_sprite_->draw(graphics, 0, 0);
	palette_->draw(graphics);
}