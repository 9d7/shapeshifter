#pragma once

#include <memory>
#include "Model.hpp"
#include "SDL_events.h"
#include "glm/fwd.hpp"

class Controller {

	public:
		Controller(std::weak_ptr<Model> model_);

		void update(float elapsed);
		bool handle_event(const SDL_Event &evt, const glm::uvec2 &window_size);

	protected:
		std::weak_ptr<Model> model;
};
