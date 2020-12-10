#include "MenuMode.hpp"
#include "Animation.hpp"
#include "Controller.hpp"
#include "SDL_events.h"
#include "SDL_scancode.h"
#include "View.hpp"
#include <memory>
#include "PlayMode.hpp"

MenuMode::~MenuMode() {}
MenuMode::MenuMode(Message m) {
	view = std::make_shared<View>();
	view->draw_ui = false;

	sprites.push_back(view->ui->on_field.from_anim(Animation::find_static("logo"), true));
	sprites.back()->set_position(glm::vec2(View::FieldWidth / 2.0f, View::FieldHeight * 0.65f));
	sprites.push_back(view->ui->on_field.from_anim(Animation::find_static("p_to_play"), true));
	sprites.back()->set_position(glm::vec2(View::FieldWidth / 2.0f, View::FieldHeight * 0.25f));

	if (m == Died) {
		sprites.push_back(view->ui->on_field.from_anim(Animation::find_dynamic("died"), true));
	} else if (m == Win) {
		sprites.push_back(view->ui->on_field.from_anim(Animation::find_dynamic("win"), true));
	}
	if (m == None) return;

	sprites.back()->set_position(glm::vec2(View::FieldWidth / 2.0f, View::FieldHeight * 0.3f));

}

void MenuMode::draw(const glm::uvec2 &drawable_size) {
	view->draw(drawable_size);
}

void MenuMode::update(float elapsed) {
	view->update(elapsed);
}

bool MenuMode::handle_event(const SDL_Event &evt, const glm::uvec2 &window_size) {

	if (evt.type == SDL_MOUSEMOTION) {
		glm::vec2 mouse_pos = Controller::fix_mouse(window_size, evt.motion.x, evt.motion.y);
		view->ui->set_cursor(mouse_pos);

		view->update_camera(0.8f * mouse_pos + glm::vec2(1000.0f, 1000.0f));

		return true;
	}

	else if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.scancode == SDL_SCANCODE_P) {
			Mode::set_current(std::make_shared<PlayMode>(shared_from_this()));
		}
	}

	return false;

}
