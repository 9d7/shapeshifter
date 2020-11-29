#include "Controller.hpp"
#include "Model.hpp"
#include "SDL_scancode.h"
#include "glm/geometric.hpp"
#include <glm/glm.hpp>
#include <memory>

Controller::Controller(std::shared_ptr<Model> model_) : model(model_) {}

void Controller::update(float elapsed) {

	glm::vec2 player_force(0.0f, 0.0f);
	for (SDL_Scancode s : pressed) {
		switch (s) {

			// handle player movement
			case SDL_SCANCODE_W: player_force += glm::vec2( 0.0f,  1.0f); break;
			case SDL_SCANCODE_A: player_force += glm::vec2(-1.0f,  0.0f); break;
			case SDL_SCANCODE_S: player_force += glm::vec2( 0.0f, -1.0f); break;
			case SDL_SCANCODE_D: player_force += glm::vec2( 1.0f,  0.0f); break;
			default: break;

		}
	}

	if (player_force != glm::vec2(0.0f, 0.0f)) {
		player_force = glm::normalize(player_force);
	}
	model->control_player(player_force);
}

bool Controller::handle_event(
	const SDL_Event &evt,
	const glm::uvec2 &window_size
) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.repeat) return false; // we hate key repeats >:(

		pressed.insert(evt.key.keysym.scancode);

		// do other stuff here if you need to trigger an event on
		// a specific keypress--things that deal with holding a key
		// should be handled in update
		return true;

	}

	if (evt.type == SDL_KEYUP) {
		pressed.erase(evt.key.keysym.scancode);
	}

	return false;

}
