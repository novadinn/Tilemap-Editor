#include "editor.h"

#include <fstream>

#include "graphics.h"
#include "canvas.h"
#include "ui_elements.h"

namespace {
	const Rectangle kCanvasBounds(144, 40, 477, 401);
	const Rectangle kPaletteBounds(24, 48, 96, 384);

	const Rectangle kPaletteSize(24, 48, 16, 64);
	const int kPaletteStartScale = 5;

	const float kScaleAmount = 0.1f;
}

Editor::Editor(Graphics& graphics) :
	background_sprite_(std::make_shared<Sprite>(graphics, "background", 0, 0, Graphics::kScreenWidth, Graphics::kScreenHeight)),
	palette_sprite_(std::make_shared<Sprite>(graphics, "palette", 0, 0, kPaletteBounds.width(), kPaletteBounds.height())),
	current_color_(0),
	editor_mode_(NONE),
	canvas_tile_size_(1),
	choosed_tile_row_(0), choosed_tile_col_(0) {
}

void Editor::startMove(int x, int y) {
	if (canvas_) {
		if (kCanvasBounds.pointIntersection(x, y)) {
			canvas_->startMoving(x, y);
		}
	}
	if (palette_) {
		if (kPaletteBounds.pointIntersection(x, y)) {
			palette_->startMoving(x, y);
		}
	}
}

void Editor::move(int x, int y, int previous_x, int previous_y) {
	if (canvas_) {
		canvas_->move(x, previous_x, y, previous_y);
		canvas_->snapToBounds(kCanvasBounds);
	}
	if (palette_) {
		palette_->move(x, previous_x, y, previous_y);
		palette_->snapToBounds(kPaletteBounds);
	}
}

void Editor::stopMove() {
	if (canvas_) {
		canvas_->stopMoving();
	}
	if (palette_) {
		palette_->stopMoving();
	}
}

void Editor::scale(float scale, int x, int y) {
	if (canvas_) {
		if (kCanvasBounds.pointIntersection(x, y)) {
			canvas_->scale(scale * kScaleAmount, x, y);
			canvas_->snapToBounds(kCanvasBounds);
		}
	}
	if (palette_) {
		if (kPaletteBounds.pointIntersection(x, y)) {
			palette_->scale(scale * kScaleAmount, x, y);
			palette_->snapToBounds(kPaletteBounds);
		}
	}
}

void Editor::putPixel(int x, int y) {
	if (canvas_) {
		if (kCanvasBounds.pointIntersection(x, y)) {
			if (editor_mode_ == TILE_SHEET) {
				canvas_->drawToTexture(x, y, current_color_);
			} else if (editor_mode_ == TILE_MAP) {
				float x_world, y_world;
				canvas_->screenToWorld(x, y, x_world, y_world);
				if (palette_ && canvas_->pointSpriteIntersection((int)x_world, (int)y_world)) {
					SDL_Surface* palette_surface = palette_->get_surface();
					SDL_Rect source_rectangle;
					source_rectangle.x = (int)choosed_tile_row_;
					source_rectangle.y = (int)choosed_tile_col_;
					source_rectangle.w = canvas_tile_size_;
					source_rectangle.h = canvas_tile_size_;

					SDL_Surface* canvas_surface = canvas_->get_surface();
					x_world = floor(x_world);
					x_world -= float(fmod(x_world, canvas_tile_size_));
					y_world = floor(y_world);
					y_world -= float(fmod(y_world, canvas_tile_size_));
					SDL_Rect destination_rectangle;
					destination_rectangle.x = (int)x_world;
					destination_rectangle.y = (int)y_world;
					destination_rectangle.w = canvas_tile_size_;
					destination_rectangle.h = canvas_tile_size_;

					const int tile_index = ((((int)choosed_tile_col_ * (palette_->get_width() / canvas_tile_size_)) + (int)choosed_tile_row_) / canvas_tile_size_);
					tile_grid_[(int)y_world/canvas_tile_size_][(int)x_world/canvas_tile_size_] = tile_index;

					SDL_BlitSurface(palette_surface, &source_rectangle, canvas_surface, &destination_rectangle);
				}
			}
		}
	}
	if (palette_) {
		if (kPaletteBounds.pointIntersection(x, y)) {
			if (editor_mode_ == TILE_SHEET) {
				palette_->drawToTexture(x, y, current_color_);
			} else if (editor_mode_ == TILE_MAP) {
				float x_world, y_world;
				palette_->screenToWorld(x, y, x_world, y_world);
				if (palette_->pointSpriteIntersection((int)x_world, (int)y_world)) {
					choosed_tile_row_ = x_world;
					choosed_tile_col_ = y_world;
					choosed_tile_row_ = floor(choosed_tile_row_);
					choosed_tile_row_ -= float(fmod(choosed_tile_row_, canvas_tile_size_));
					choosed_tile_col_ = floor(choosed_tile_col_);
					choosed_tile_col_ -= float(fmod(choosed_tile_col_, canvas_tile_size_));
				}
			}
		}
	}
}

void Editor::set_color(Uint32 color) {
	current_color_ = color;
}

void Editor::setColorAtPoint(int x, int y) {
	if (canvas_ && canvas_->getPixel(x, y) != std::nullopt) {
		current_color_ = *canvas_->getPixel(x, y);
	} else if (palette_ && palette_->getPixel(x, y) != std::nullopt) {
		current_color_ = *palette_->getPixel(x, y);
	}
}

void Editor::createTileSheet(Graphics& graphics, int size) {
	canvas_.reset(new Canvas(graphics, Graphics::kScreenWidth * 1.0f / 2, Graphics::kScreenHeight * 1.0f / 2, size, size));

	canvas_tile_size_ = size;

	editor_mode_ = TILE_SHEET;
}

void Editor::createTileMap(Graphics& graphics, std::string& tile_sheet_name, int x_count, int y_count) {
	std::ifstream file("content/images/" + tile_sheet_name + ".txt");
	std::string size;
	std::getline(file, size);

	canvas_.reset(new Canvas(graphics, Graphics::kScreenWidth * 1.0f / 2, Graphics::kScreenHeight * 1.0f / 2, stoi(size) * x_count, stoi(size) * y_count));
	palette_.reset();

	choosed_tile_row_ = 0;
	choosed_tile_col_ = 0;
	canvas_tile_size_ = stoi(size);

	editor_mode_ = TILE_MAP;
	loadTileSheetAsPalette(graphics, tile_sheet_name);

	tile_grid_ = std::vector<std::vector<int>>(y_count, std::vector<int>(x_count, int()));
	for (size_t row = 0; row < tile_grid_.size(); ++row) {
		for (size_t col = 0; col < tile_grid_[row].size(); ++col) {
			tile_grid_[row][col] = 0;
		}
	}
}

void Editor::createPalette(Graphics& graphics) {
	if (editor_mode_ == TILE_SHEET) {
		palette_.reset(new Canvas(graphics, kPaletteSize.left() * 1.0f, kPaletteSize.top() * 1.0f, kPaletteSize.width(), kPaletteSize.height()));
		palette_->scale(kPaletteStartScale, kPaletteSize.left(), kPaletteSize.top());
	}
}

void Editor::saveCanvas(Graphics& graphics, const std::string& file_path) {
	if (canvas_) {
		canvas_->save(graphics, file_path + ".bmp");

		SDL_RWops* io = SDL_RWFromFile((file_path + ".txt").c_str(), "w+");
		
		if (editor_mode_ == TILE_SHEET) {
			std::string str = std::to_string(canvas_tile_size_);

			int tile_index = 0;
			for (int y = 0; y < canvas_->get_height(); y += canvas_tile_size_) {
				for (int x = 0; x < canvas_->get_width(); x += canvas_tile_size_) {
					str += "\n" + std::to_string(tile_index) + ": ";
					for (int x_world = 0; x_world < canvas_tile_size_; ++x_world) {
						for (int y_world = 0; y_world < canvas_tile_size_; ++y_world) {
							SDL_Surface* sprite_sheet = canvas_->get_surface();
							Uint8* pixel_array = (Uint8*)sprite_sheet->pixels_;
							Uint8 red = pixel_array[(y + y_world) * sprite_sheet->pitch + (x + x_world) * sprite_sheet->format->BytesPerPixel + 2];
							Uint8 green = pixel_array[(y + y_world) * sprite_sheet->pitch + (x + x_world) * sprite_sheet->format->BytesPerPixel + 1];
							Uint8 blue = pixel_array[(y + y_world) * sprite_sheet->pitch + (x + x_world) * sprite_sheet->format->BytesPerPixel + 0];

							if (SDL_MapRGB(sprite_sheet->format, red, green, blue) != SDL_MapRGB(sprite_sheet->format, 0, 0, 0)) {
								str += std::to_string(canvas_tile_size_ - y_world) + " ";
								break;
							} else if (y_world == canvas_tile_size_ - 1) {
								str += std::to_string(0) + " ";
							}
						}
 					}
					++tile_index;
				}
			}

			size_t length = SDL_strlen(str.c_str());
			SDL_RWwrite(io, str.c_str(), 1, length);
		} else if (editor_mode_ == TILE_MAP) {
			std::string str = std::to_string(canvas_tile_size_) + "\n";

			for (size_t row = 0; row < tile_grid_.size(); ++row) {
				for (size_t col = 0; col < tile_grid_[row].size(); ++col) {
					str += std::to_string(tile_grid_[row][col]);
				}
				str += "\n";
			}

			size_t length = SDL_strlen(str.c_str());
			SDL_RWwrite(io, str.c_str(), 1, length);
		}

		SDL_RWclose(io);
	}
}

void Editor::loadTileSheetAsCanvas(Graphics& graphics, const std::string& file_path) {
	canvas_.reset(new Canvas(graphics, file_path, Graphics::kScreenWidth * 1.0f/2, Graphics::kScreenHeight * 1.0f/2));
	palette_.reset();

	editor_mode_ = TILE_SHEET;

	std::ifstream file("content/images/" + file_path + ".txt");
	std::string size;
	std::getline(file, size);
	canvas_tile_size_ = stoi(size);
}

void Editor::loadTileMap(Graphics& graphics, const std::string& file_path) {
	canvas_.reset(new Canvas(graphics, file_path, Graphics::kScreenWidth * 1.0f / 2, Graphics::kScreenHeight * 1.0f / 2));
	palette_.reset();

	editor_mode_ = TILE_MAP;

	std::ifstream file("content/images/" + file_path + ".txt");
	std::string line;
	std::getline(file, line);
	canvas_tile_size_ = stoi(line);

	for (line; std::getline(file, line);) {
		std::vector<int> row;
		for (size_t i = 0; i < line.size(); ++i) {
			if ((char*)line[i] != " ") {
				row.push_back(line[i] - '0');
			}
		}
		tile_grid_.push_back(row);
	}
}

void Editor::loadTileSheetAsPalette(Graphics& graphics, const std::string& file_path) {
	if (!canvas_ || editor_mode_ != TILE_MAP) {
		return;
	}
	palette_.reset(new Canvas(graphics, file_path, kPaletteSize.left() * 1.0f, kPaletteSize.top() * 1.0f));
	editor_mode_ = TILE_MAP;
}

void Editor::extendCanvasX(Graphics& graphics) {
	if (canvas_) {
		canvas_->changeSize(graphics, canvas_tile_size_, 0);
		canvas_->snapToBounds(kCanvasBounds);

		if (editor_mode_ == TILE_MAP) {
			const int height = canvas_->get_height() / canvas_tile_size_;
			for (int i = 0; i < height; ++i) {
				tile_grid_[i].push_back(0);
			}
		}
	}
}

void Editor::extendCanvasY(Graphics& graphics) {
	if (canvas_) {
		canvas_->changeSize(graphics, 0, canvas_tile_size_);
		canvas_->snapToBounds(kCanvasBounds);

		if (editor_mode_ == TILE_MAP) {
			const int width = canvas_->get_width() / canvas_tile_size_;
			std::vector<int> new_col;
			for (int i = 0; i < width; ++i) {
				new_col.push_back(0);
			}
			tile_grid_.push_back(new_col);
		}
	}
}

void Editor::truncateCanvasX(Graphics& graphics) {
	if (canvas_) {
		if (canvas_->get_width() == canvas_tile_size_)
			return;

		canvas_->changeSize(graphics, -canvas_tile_size_, 0);
		canvas_->snapToBounds(kCanvasBounds);

		if (editor_mode_ == TILE_MAP) {
			const int height = canvas_->get_height() / canvas_tile_size_;
			for (int i = 0; i < height; ++i) {
				tile_grid_[i].pop_back();
			}
		}
	}
}

void Editor::truncateCanvasY(Graphics& graphics) {
	if (canvas_) {
		if (canvas_->get_height() == canvas_tile_size_)
			return;

		canvas_->changeSize(graphics, 0, -canvas_tile_size_);
		canvas_->snapToBounds(kCanvasBounds);

		if (editor_mode_ == TILE_MAP) {
			tile_grid_.pop_back();
		}
	}
}

void Editor::draw(Graphics& graphics) const {
	if (canvas_) {
		canvas_->draw(graphics);
		canvas_->drawGrid(graphics, canvas_tile_size_, canvas_tile_size_);
	}
	palette_sprite_->draw(graphics, kPaletteBounds.left(), kPaletteBounds.top());
	if (palette_) {

		palette_->draw(graphics);
		palette_->drawGrid(graphics, canvas_tile_size_, canvas_tile_size_);

		if (editor_mode_ == TILE_MAP) {
			int screen_left, screen_top;
			palette_->worldToScreen(choosed_tile_row_, choosed_tile_col_, screen_left, screen_top);
			int screen_right, screen_bottom;
			palette_->worldToScreen(choosed_tile_row_ + canvas_tile_size_, choosed_tile_col_ + canvas_tile_size_, screen_right, screen_bottom);
			const Uint32 color = (255 << 16) | (0 << 8) | 0;
			graphics.drawLine(screen_left, screen_top, screen_right, 0, color);
			graphics.drawLine(screen_left, screen_top, 0, screen_bottom, color);
			graphics.drawLine(screen_left, screen_bottom, screen_right, 0, color);
			graphics.drawLine(screen_right, screen_top, 0, screen_bottom, color);
		}
	}
	background_sprite_->draw(graphics, 0, 0);
}