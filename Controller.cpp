#include "Controller.hpp"
#include "MenuMode.hpp"
#include "Mode.hpp"
#include "Model.hpp"
#include "SDL_events.h"
#include "SDL_scancode.h"
#include "glm/geometric.hpp"
#include <glm/glm.hpp>
#include <memory>

Controller::Controller(std::shared_ptr<Model> model_) : model(model_) {}

glm::vec2 Controller::fix_mouse(const glm::uvec2 &window_size, int x, int y){

	glm::vec2 screen_pos(x, window_size.y - y);

	// get real drawable size
	size_t actual_window_scale = std::min(
		window_size.x * View::ScreenHeight,
		window_size.y * View::ScreenWidth
	);
	glm::uvec2 actual_window_size = glm::uvec2(
		actual_window_scale / View::ScreenHeight,
		actual_window_scale / View::ScreenWidth
	);

	// map(x, (drawable_size.x - actual_drawable_size.x) / 2.0f, (drawable_size.x + actual_drawable_size.x) / 2.0f, 0, ScreenWidth)
	glm::vec2 mouse_position;
	mouse_position.x = (screen_pos.x - (window_size.x - actual_window_size.x) / 2.0f) * (float)View::ScreenWidth  / actual_window_size.x;
	mouse_position.y = (screen_pos.y - (window_size.y - actual_window_size.y) / 2.0f) * (float)View::ScreenHeight / actual_window_size.y;
	return mouse_position;
};

void Controller::update(float elapsed) {

	glm::vec2 player_force(0.0f, 0.0f);
	bool lock_rotation = false;
	for (SDL_Scancode s : pressed) {
		switch (s) {

			// handle player movement
			case SDL_SCANCODE_W: player_force += glm::vec2( 0.0f,  1.0f); break;
			case SDL_SCANCODE_A: player_force += glm::vec2(-1.0f,  0.0f); break;
			case SDL_SCANCODE_S: player_force += glm::vec2( 0.0f, -1.0f); break;
			case SDL_SCANCODE_D: player_force += glm::vec2( 1.0f,  0.0f); break;
			case SDL_SCANCODE_ESCAPE: Mode::set_current(std::make_shared<MenuMode>(MenuMode::None)); return;
			case SDL_SCANCODE_SPACE: lock_rotation = true;
			default: break;

		}
	}

	if (player_force != glm::vec2(0.0f, 0.0f)) {
		player_force = glm::normalize(player_force);
	}
	model->player_move(player_force);
	model->lock_rotation(lock_rotation);
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
		return true;
	}


	if (evt.type == SDL_MOUSEMOTION) {
		model->set_mouse_position(fix_mouse(window_size, evt.motion.x, evt.motion.y));
	}

	if (evt.type == SDL_MOUSEBUTTONDOWN) {

		model->set_mouse_position(fix_mouse(window_size, evt.button.x, evt.button.y));

		if (evt.button.button == SDL_BUTTON_LEFT) {

			model->set_player_color(Bullet::Color::Blue);
			model->player_shoot();

		} else if (evt.button.button == SDL_BUTTON_RIGHT) {

			model->set_player_color(Bullet::Color::Red);
			model->player_shoot();

		}

	}

	return false;

}
