#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "Renderer.hpp"
#include "SDL_events.h"
#include "SDL_keyboard.h"
#include "SDL_mouse.h"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/geometric.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <random>

Load < Renderer > renderer_(LoadTagDefault, []() -> Renderer const * {
	return new Renderer();
});

PlayMode::PlayMode() : renderer(*renderer_) {
	SDL_ShowCursor(SDL_DISABLE);
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	SDL_KeyCode key = (SDL_KeyCode)evt.key.keysym.sym;
	if (evt.type == SDL_KEYDOWN) {
		if (buttons.find(key) != buttons.end()) {
			buttons[key].downs += 1;
			buttons[key].pressed = true;
			return true;
		}
	}
	else if (evt.type == SDL_KEYUP) {
		if (buttons.find(key) != buttons.end()) {
			buttons[key].pressed = false;
			return true;
		}
	}
	else if (evt.type == SDL_MOUSEMOTION) {
		SDL_MouseMotionEvent mme = evt.motion;
		glm::vec2 screen_pos {(float)mme.x, (float)(window_size.y - mme.y)};

		// get real drawable size
		size_t actual_window_scale = std::min(
			window_size.x * Renderer::ScreenHeight,
			window_size.y * Renderer::ScreenWidth
		);
		glm::uvec2 actual_window_size = glm::uvec2(
			actual_window_scale / Renderer::ScreenHeight,
			actual_window_scale / Renderer::ScreenWidth
		);

		// map(x, (drawable_size.x - actual_drawable_size.x) / 2.0f, (drawable_size.x + actual_drawable_size.x) / 2.0f, 0, ScreenWidth)
		mouse_position.x = (screen_pos.x - (window_size.x - actual_window_size.x) / 2.0f) * (float)Renderer::ScreenWidth  / actual_window_size.x;
		mouse_position.y = (screen_pos.y - (window_size.y - actual_window_size.y) / 2.0f) * (float)Renderer::ScreenHeight / actual_window_size.y;

		mouse_vector_from_player = mouse_position + camera_position - player_position - PIXEL_SCREEN_CENTER;

		renderer.update_cursor_position(mouse_position);

		return true;
	}
	else if (evt.type == SDL_MOUSEBUTTONDOWN) {
        if (evt.button.button == SDL_BUTTON_LEFT) { //lmb for blue
            player_color = Renderer::Blue;
        } else if (evt.button.button == SDL_BUTTON_RIGHT) { //rmb for red
            player_color = Renderer::Red;
        }
        shoot_bullet();
        return true;
    }


	return false;
}

void PlayMode::update(float elapsed) {

	// Do actions

	// Comment out the following line to allow for linear acceleration and exponential velocity
	force_vector = glm::vec2(0, 0);
	update_force_vector(force_vector);

	// Clear downs after all key presses are accounted for
	//reset_downs();

	player_velocity += force_vector * elapsed;

	// velocity cap
	if (glm::length(player_velocity) > MAX_VELOCITY) {
		player_velocity = glm::normalize(player_velocity) * MAX_VELOCITY;
	}

	//update position
	player_position += player_velocity * elapsed;
	player_velocity *= FRICTION;
	mouse_vector_from_player = mouse_position + camera_position - player_position - PIXEL_SCREEN_CENTER;


	//std::cout << "player: " << glm::to_string(player_position) << " | mouse: " << glm::to_string(mouse_position) << " | camera: " << glm::to_string(camera_position) << " \nattempt: " << glm::to_string(mouse_vector_from_player) << "\n";

	// Update player rotation
	if (glm::length(player_velocity) > 0) {
		//renderer.update_char_position(player_position, glm::atan(player_velocity.y, player_velocity.x)); // Velocity based rotation
		renderer.update_char_position(player_position, glm::atan(mouse_vector_from_player.y, mouse_vector_from_player.x)); // Mouse position based rotation
	} else {
		renderer.update_char_position(player_position, 0);
	}

	//delete while iterating taken from
	//https://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
    std::deque<bullet_wrapper>::iterator i = bullets.begin();
    while (i != bullets.end()) {
        i->b->elapsed += elapsed;
        // also check collision
        if (i->b->elapsed > max_bullet_time) {
            renderer.destroy_bullet(i->b);
            bullets.erase(i++);  // alternatively, i = items.erase(i);
            if (bullets.size() == 0) break;
        } else {
            i->b->position += i->direction * elapsed * 200.0f;
            renderer.update_bullet_position(i->b, i->b->position);
            ++i;
        }
	}


	// update camera to be out of dead space
	static const glm::vec2 MARGIN = glm::vec2(
			Renderer::ScreenWidth,
			Renderer::ScreenHeight
	) * 1.0f / 3.0f;

	glm::vec2 space_from_camera_center = camera_position - player_position;

	if (space_from_camera_center.x > MARGIN.x) {
		camera_position.x = MARGIN.x + player_position.x;
	}
	if (-space_from_camera_center.x > MARGIN.x) {
		camera_position.x = player_position.x - MARGIN.x;
	}

	if (space_from_camera_center.y > MARGIN.y) {
		camera_position.y = MARGIN.y + player_position.y;
	}
	if (-space_from_camera_center.y > MARGIN.y) {
		camera_position.y = player_position.y - MARGIN.y;
	}

	renderer.update_camera_position(camera_position);
	renderer.update(elapsed);
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	renderer.draw(drawable_size);
	GL_ERRORS();
}

void PlayMode::update_force_vector(glm::vec2& force_vector)
{
	if (buttons[SDLK_w].pressed) {
		force_vector.y += 1.0f;
	}
	if (buttons[SDLK_a].pressed) {
		force_vector.x -= 1.0f;
	}
	if (buttons[SDLK_s].pressed) {
		force_vector.y -= 1.0f;
	}
	if (buttons[SDLK_d].pressed) {
		force_vector.x += 1.0f;
	}

	// apply force
	if (glm::length(force_vector) > 0) {
		force_vector = glm::normalize(force_vector) * MOVE_FORCE;
	}
}

void PlayMode::reset_downs()
{
	for (std::pair<const SDL_KeyCode, Button> &button : buttons) {
		button.second.downs = 0;
	}
	return;
}

void PlayMode::shoot_bullet() {
    glm::vec2 shoot_at = glm::vec2(mouse_position.x - Renderer::ScreenWidth/2 + camera_position.x, mouse_position.y - Renderer::ScreenHeight/2 + camera_position.y);
    Renderer::Bullet b = renderer.new_bullet(player_position, player_color);
    bullet_wrapper new_bullet;
    b->elapsed = 0.0f;
    new_bullet.b = b;
    new_bullet.direction = glm::vec2(shoot_at.x - player_position.x, shoot_at.y - player_position.y);
    new_bullet.direction = glm::normalize(new_bullet.direction);
    bullets.push_back(new_bullet);
}
