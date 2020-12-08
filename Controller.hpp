#pragma once

#include <memory>
#include "Model.hpp"
#include "SDL_events.h"
#include "SDL_scancode.h"
#include "glm/fwd.hpp"
#include <unordered_set>

class Controller {

	public:


		Controller(std::shared_ptr<Model> model_);

		void update(float elapsed);
		bool handle_event(const SDL_Event &evt, const glm::uvec2 &window_size);
		static glm::vec2 fix_mouse(const glm::uvec2 &window_size, int x, int y);

	protected:
		std::shared_ptr<Model> model;

		// @pablo: i know scancodes are longer to type,
		// but i checked the docs and they're what you're supposed
		// to use for location-independent keys, see:
		// https://stackoverflow.com/questions/31941446/what-is-the-difference-between-an-sdl-physical-key-code-and-an-sdl-virtual-key-c
		std::unordered_set<SDL_Scancode> pressed;
};
