#include "application.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include "graphics.h"
#include "input.h"
#include "interaction_system.h"

namespace {
	const int kDeltaTime = 1;
	const int kGameSpeed = 1;
}

Application::Application() {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	eventLoop();
}

Application::~Application() {
	SDL_Quit();
	TTF_Quit();
}

void Application::eventLoop() {
	Graphics graphics;
	Input input;
	SDL_Event event;

	interaction_system_.reset(InteractionSystem::createSystem(graphics));

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
				default:
					break;
				}
			}

			if (input.wasKeyPressed(SDLK_ESCAPE)) {
				running = false;
			}

			// update
			int mouse_x, mouse_y;
			SDL_GetMouseState(&mouse_x, &mouse_y);

			interaction_system_->checkForInteractions(graphics, input, mouse_x, mouse_y, previous_mouse_x, previous_mouse_y);

			frame_time_accumulator -= kDeltaTime;
		}

		draw(graphics);
	}
}

void Application::draw(Graphics& graphics) {
	graphics.clear();

	interaction_system_->draw(graphics);

	graphics.flip();
}
