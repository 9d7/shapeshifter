#pragma once

#include "Mode.hpp"

#include "Model.hpp"
#include "Sprite.hpp"
#include "View.hpp"
#include "Controller.hpp"
#include <memory>
#include <vector>

struct MenuMode : Mode {
	public:

		enum Message {
			None,
			Died,
			Win
		};

		MenuMode(Message m);
		virtual ~MenuMode();

		//functions called by main loop:
		virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
		virtual void update(float elapsed) override;
		virtual void draw(glm::uvec2 const &drawable_size) override;

		std::shared_ptr<View> view = nullptr;

		std::vector<std::shared_ptr<Sprite>> sprites;

};
