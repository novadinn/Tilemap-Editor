#include "application.h"

#include <functional>
#include <iostream>
#include <string>

#include "editor.h"
#include "input.h"
#include "surface_window.h"
#include "render_window.h"
#include "imgui\\imgui.h"
#include "imgui\\imgui_impl_sdl.h"
#include "imgui\\imgui_impl_sdlrenderer.h"

namespace {
	const int kDeltaTime = 1;
	const int kGameSpeed = 1;

	const int kScrollSpeed = 2;
}

Application::Application() {
	SDL_Init(SDL_INIT_EVERYTHING);
	eventLoop();
}

Application::~Application() {
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_Quit();
}

void Application::eventLoop() {
	RenderWindow render_window;
	SurfaceWindow canvas_window(640, 480, 0);
	SurfaceWindow palette_window(120, 480, SDL_WINDOW_SKIP_TASKBAR);

	Input input;
	SDL_Event event;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsLight();
	ImGui_ImplSDL2_InitForSDLRenderer(render_window.get_window(), render_window.get_renderer());
	ImGui_ImplSDLRenderer_Init(render_window.get_renderer());

	current_color_ = 0;

	editor_.reset(new Editor());

	bool render_window_focused = false;
	int last_update_time = SDL_GetTicks();
	int frame_time_accumulator = 0;
	bool running = true;
	while (running) {
		const int start_frame_time = SDL_GetTicks();
		const int frame_time = start_frame_time - last_update_time;
		last_update_time = start_frame_time;
		frame_time_accumulator += frame_time;

		while (frame_time_accumulator >= kDeltaTime) {
			input.beginNewFrame();
			int previous_mouse_x, previous_mouse_y;
			SDL_GetMouseState(&previous_mouse_x, &previous_mouse_y);

			while (SDL_PollEvent(&event)) {
				if(render_window_focused)
					ImGui_ImplSDL2_ProcessEvent(&event);

				switch (event.type) {
				case SDL_KEYDOWN:
					if (!event.key.repeat)
						input.keyDownEvent(event);
					break;
				case SDL_KEYUP:
					input.keyUpEvent(event);
					break;
				case SDL_MOUSEBUTTONDOWN:
					input.mouseDownEvent(event);
					break;
				case SDL_MOUSEBUTTONUP:
					input.mouseUpEvent(event);
					break;
				case SDL_MOUSEWHEEL:
					input.mouseWheelEvent(event);
					break;
				case SDL_WINDOWEVENT:
					switch (event.window.event) {
					case SDL_WINDOWEVENT_MOVED:
						if (event.window.windowID == canvas_window.get_window_id()) {
							palette_window.set_position(event.window.data1, event.window.data2);
						}
						break;
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						if (event.window.windowID == canvas_window.get_window_id()) {
							palette_window.raise();
						} else if (event.window.windowID == render_window.get_window_id()) {
							render_window_focused = true;
						}
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						if (event.window.windowID == render_window.get_window_id()) {
							render_window_focused = false;
						}
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
			}

			if (input.wasKeyPressed(SDLK_ESCAPE)) {
				running = false;
			}

			int canvas_mouse_x, canvas_mouse_y;
			SDL_GetGlobalMouseState(&canvas_mouse_x, &canvas_mouse_y);
			int canvas_window_x, canvas_window_y;
			canvas_window.get_window_position(canvas_window_x, canvas_window_y);
			canvas_mouse_x -= canvas_window_x;
			canvas_mouse_y -= canvas_window_y;

			if (editor_) {
				if (input.wasMouseButtonPressed(SDL_BUTTON_MIDDLE)) {
					editor_->startMove(canvas_mouse_x, canvas_mouse_y);
				} else if (input.isMouseButtonPressed(SDL_BUTTON_MIDDLE)) {
					editor_->move(canvas_mouse_x, canvas_mouse_y, previous_mouse_x, previous_mouse_y);
				} else if (input.wasMouseButtonReleased(SDL_BUTTON_MIDDLE)) {
					editor_->stopMove();
				}

				if (input.isMouseButtonPressed(SDL_BUTTON_LEFT)) {
					editor_->putPixel(current_color_, canvas_mouse_x, canvas_mouse_y);
				} else if (input.isMouseButtonPressed(SDL_BUTTON_RIGHT)) {
					if (editor_->get_color_at_point(canvas_mouse_x, canvas_mouse_y) != std::nullopt) {
						current_color_ = *editor_->get_color_at_point(canvas_mouse_x, canvas_mouse_y);
					}
				}

				if (input.getWheelMovement() != 0) {
					editor_->scale(input.getWheelMovement() * kScrollSpeed, canvas_mouse_x, canvas_mouse_y);
				}
			}

			frame_time_accumulator -= kDeltaTime;
		}

		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		static int canvas_size = 1;
		ImGui::Begin("ImGui");

		if (ImGui::Button("Create tile sheet")) {
			editor_->createTileSheet(canvas_window, canvas_size);
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(100);
		ImGui::DragInt("X/Y", &canvas_size, 0.1f, 1, 2147483647);

		static int tile_map_size[2] = { 1, 1 };
		static char buffer_tile_size[256] = {};
		if (ImGui::Button("Create tile map")) {
			std::string str = "content/images/";
			str += buffer_tile_size;
			editor_->createTileMap(canvas_window, palette_window, str, tile_map_size[0], tile_map_size[1]);
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(100);
		ImGui::DragInt2("X/Y", tile_map_size, 0.1f, 1, 2147483647);
		ImGui::SameLine();
		ImGui::InputText("Tile sheet file path", buffer_tile_size, sizeof(buffer_tile_size));

		if (ImGui::Button("Create palette")) {
			editor_->createPalette(palette_window);
		}
		if (ImGui::Button("Extend X")) {
			editor_->extendCanvasX(canvas_window);
		}
		if (ImGui::Button("Extend Y")) {
			editor_->extendCanvasY(canvas_window);
		}
		if (ImGui::Button("Truncate X")) {
			editor_->truncateCanvasX(canvas_window);
		}
		if (ImGui::Button("Truncate Y")) {
			editor_->truncateCanvasY(canvas_window);
		}

		static float color_float[3] = { 0, 0, 0 };
		if (ImGui::ColorPicker3("Color", color_float, 0)) {
			current_color_ = ((int(color_float[2] * 255)) << 16) | ((int(color_float[1] * 255)) << 8) | int(color_float[0] * 255);
		}

		char buffer_save_file[256] = {};
		if (ImGui::InputText("Save file", buffer_save_file, sizeof(buffer_save_file), ImGuiInputTextFlags_EnterReturnsTrue)) {
			editor_->saveCanvas(canvas_window, "content/images/" + static_cast<std::string>(buffer_save_file));
		}
		char buffer_load_tile_sheet_as_canvas[256] = {};
		if (ImGui::InputText("Load tile sheet as canvas", buffer_load_tile_sheet_as_canvas, sizeof(buffer_load_tile_sheet_as_canvas), ImGuiInputTextFlags_EnterReturnsTrue)) {
			editor_->loadTileSheetAsCanvas(canvas_window, "content/images/" + static_cast<std::string>(buffer_load_tile_sheet_as_canvas));
		}
		char buffer_load_tile_sheet_as_palette[256] = {};
		if (ImGui::InputText("Load tile sheet as palette", buffer_load_tile_sheet_as_palette, sizeof(buffer_load_tile_sheet_as_palette), ImGuiInputTextFlags_EnterReturnsTrue)) {
			editor_->loadTileSheetAsPalette(palette_window, "content/images/" + static_cast<std::string>(buffer_load_tile_sheet_as_palette));
		}
		char buffer_load_tile_map[256] = {};
		if (ImGui::InputText("Load tile map", buffer_load_tile_map, sizeof(buffer_load_tile_map), ImGuiInputTextFlags_EnterReturnsTrue)) {
			editor_->loadTileMap(canvas_window, "content/images/" + static_cast<std::string>(buffer_load_tile_map));
		}

		ImGui::End();

		ImGui::Render();
		render_window.clear();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		render_window.flip();

		canvas_window.clear();
		palette_window.clear();
		
		editor_->draw(canvas_window, palette_window);
		
		palette_window.flip();
		canvas_window.flip();

	}
}