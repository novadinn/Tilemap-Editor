#pragma once

#include <memory>

struct Graphics;
struct InteractionSystem;

struct Application {
	Application();
	~Application();
private:
	void eventLoop();

	void draw(Graphics& graphics);

	std::unique_ptr<InteractionSystem> interaction_system_;
};