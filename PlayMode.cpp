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
	Renderer::Enemy e = renderer.new_enemy(glm::vec2(0, 15), 0, Renderer::EnemyType::Hunter, Renderer::BulletColor::Blue);
    enemy_wrapper ewrap;
	ewrap.e = e;
	enemies.push_back(ewrap);
	Renderer::Enemy e2 = renderer.new_enemy(glm::vec2(0, -15), 0, Renderer::EnemyType::Soldier, Renderer::BulletColor::Red);
	//enemies.push_back(e2);
	enemy_wrapper ewrap2;
	ewrap2.bullet_timer = 2.0f;
	ewrap2.e = e2;
	enemies.push_back(ewrap2);
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
			if (key == SDLK_p) dev_mode = !dev_mode;
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
		renderer.update_char_color(player_color);
        shoot_bullet();
        return true;
    }


	return false;
}

void PlayMode::update(float elapsed) {
	// Do actions
	force_vector = glm::vec2(0, 0);
	update_force_vector(force_vector);

	// Clear downs after all key presses are accounted for
	//reset_downs();

	player_velocity += force_vector * elapsed;

	// velocity cap
	if (glm::length(player_velocity) > MAX_VELOCITY * dev_velocity_multiplier) {
		player_velocity = glm::normalize(player_velocity) * MAX_VELOCITY * dev_velocity_multiplier;
	}

	//update position
	player_position += player_velocity * elapsed;
	player_velocity *= FRICTION;
	mouse_vector_from_player = mouse_position + camera_position - player_position - PIXEL_SCREEN_CENTER;

	// Dev mode
	if (dev_mode) {
		dev_mode_update();
	}

	// Update player rotation
	if (dev_velocity_rotation_mode) { // Velocity based rotation
		if (glm::length(player_velocity) > 0) {
			renderer.update_char_position(player_position, glm::atan(player_velocity.y, player_velocity.x));
		}
		else {
			renderer.update_char_position(player_position, 0);
		}
	}
	else { // Mouse position based rotation
		renderer.update_char_position(player_position, glm::atan(mouse_vector_from_player.y, mouse_vector_from_player.x)); 
	}
	

	update_bullets(player_bullets, elapsed);
	check_collisions();
	
	if (enemies.size() < 10) generate_enemy();

	for (size_t i = 0; i < enemies.size(); i++) {
		enemies[i].bullet_timer -= elapsed;
		if (enemies[i].e->type == Renderer::EnemyType::Soldier) {
			move_soldier(enemies[i], elapsed);
		} else if (enemies[i].e->type == Renderer::EnemyType::Hunter) {
			move_hunter(enemies[i], elapsed);
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

void PlayMode::update_force_vector(glm::vec2& force_vector) {
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
		force_vector = glm::normalize(force_vector) * MOVE_FORCE * dev_velocity_multiplier;
	}
}

// Currently unneeded as we're sticking with linear velocity
void PlayMode::reset_downs() {
	for (std::pair<SDL_KeyCode, Button> &button : buttons) {
		button.second.downs = 0;
	}
	return;
}

void PlayMode::dev_mode_update() {
	if (buttons[SDLK_r].pressed) {
		player_position = glm::vec2(0, 0);
		camera_position = glm::vec2(0, 0);
	}
	if (buttons[SDLK_j].pressed && dev_velocity_multiplier > .01f) {
		dev_velocity_multiplier -= .01f;
	}
	if (buttons[SDLK_k].pressed) {
		dev_velocity_multiplier += .01f;
	}
	if (buttons[SDLK_v].pressed) {
		dev_velocity_rotation_mode = true;
	}
	if (buttons[SDLK_m].pressed) {
		dev_velocity_rotation_mode = false;
	}
	if (buttons[SDLK_l].pressed) {
		dev_position_debug = true;
	}
	if (buttons[SDLK_h].pressed) {
		dev_show_dev_values = true;
	}
	if (buttons[SDLK_o].pressed) {
		dev_position_debug = false;
		dev_show_dev_values = false;
	}

	if (dev_position_debug) {
		std::cout << "player: " << glm::to_string(player_position) << " | mouse: " << glm::to_string(mouse_position) << " | camera: " << glm::to_string(camera_position) << " | mouse from player: " << glm::to_string(mouse_vector_from_player) << "\n";
	}
	if (dev_show_dev_values) {
		std::cout << "Velocity multiplier: " << dev_velocity_multiplier << " | Rotation mode: " << dev_velocity_rotation_mode << "\n";
	}
}

void PlayMode::shoot_bullet() {
    glm::vec2 shoot_at = glm::vec2(mouse_position.x - Renderer::ScreenWidth/2 + camera_position.x, mouse_position.y - Renderer::ScreenHeight/2 + camera_position.y);
    Renderer::Bullet b = renderer.new_bullet(player_position, player_color);
    bullet_wrapper new_bullet;
    b->elapsed = 0.0f;
    new_bullet.b = b;
	new_bullet.speed = 200;
    new_bullet.direction = glm::vec2(shoot_at.x - player_position.x, shoot_at.y - player_position.y);
    new_bullet.direction = glm::normalize(new_bullet.direction);
    player_bullets.push_back(new_bullet);
}



void PlayMode::check_collisions() {
	// Just checks player bullets and enemie bodys for now
	for (bullet_wrapper bullet : enemy_bullets) {
		glm::vec2 distance_vector = bullet.b->position - player_position;
		float dist = glm::length(distance_vector);
		if (dist < 10.0f) {
			dead = true;
		}
	}
	for (bullet_wrapper bullet : player_bullets) {
		for (Renderer::Enemy enemy : enemies) {
			glm::vec2 distance_vector = bullet.b->position - enemy->position;
			float dist = glm::length(distance_vector);
			if (dist < 10.0f) {
				renderer.destroy_enemy(enemy);
				
			}
		}
	}
	for (auto bull_it = player_bullets.begin(); bull_it != player_bullets.end(); ++bull_it) {
		for (auto enem_it = enemies.begin(); enem_it != enemies.end(); ++enem_it) {
			glm::vec2 distance_vector = enem_it
		}
	}
}

void PlayMode::update_bullets(std::deque<bullet_wrapper> &bullets, float elapsed) {
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
		}
		else {
			i->b->position += i->direction * elapsed * i->speed; // TODO replace this with bullet speed
			renderer.update_bullet_position(i->b, i->b->position);
			++i;
		}
	}

}

void PlayMode::move_soldier(enemy_wrapper &ewrap, float elapsed) {
	float xdiff = ewrap.e->position.x - player_position.x;
	float ydiff = ewrap.e->position.y - player_position.y;
	float dist = sqrt(xdiff*xdiff + ydiff*ydiff);
	glm::vec2 move_enemy = glm::normalize(player_position - ewrap.e->position);
	move_enemy = move_enemy * 50.0f * elapsed;
	if (dist > 100.0f) {
		ewrap.e->position = ewrap.e->position + move_enemy;
		renderer.update_enemy_position(ewrap.e, ewrap.e->position);
	} else if (dist < 50.0f) {
		ewrap.e->position = ewrap.e->position - move_enemy;
		renderer.update_enemy_position(ewrap.e, ewrap.e->position);
	} else if (ewrap.bullet_timer < 0.0f) {
		shoot_enemy_bullet(ewrap.e, 100.0f);
		ewrap.bullet_timer = 2.0f;
	}
}

void PlayMode::move_hunter(enemy_wrapper &ewrap, float elapsed) {
	glm::vec2 move_enemy = glm::normalize(player_position - ewrap.e->position);
	move_enemy = move_enemy * 70.0f * elapsed;
	ewrap.e->position = ewrap.e->position + move_enemy;
	renderer.update_enemy_position(ewrap.e, ewrap.e->position);
}

void PlayMode::shoot_enemy_bullet(Renderer::Enemy &e, float speed) {
    Renderer::Bullet b = renderer.new_bullet(e->position, e->color);
    bullet_wrapper new_bullet;
    b->elapsed = 0.0f;
    new_bullet.b = b;
	new_bullet.speed = 50.0f;
    new_bullet.direction = player_position - e->position;
    new_bullet.direction = glm::normalize(new_bullet.direction);
    enemy_bullets.push_back(new_bullet);

}

void PlayMode::generate_enemy() {
	int r = rand() % 4;
	float minx, maxx, miny, maxy;
	if (r == 0) {
		minx = camera_position.x + renderer.ScreenWidth / 2;
		maxx = minx + 50.0f;
		miny = camera_position.y - renderer.ScreenHeight / 2;
		maxy = camera_position.y + renderer.ScreenHeight / 2;
	} else if (r == 1) {
		minx = camera_position.x - renderer.ScreenWidth / 2 - 50.0f;
		maxx = minx + 50.0f;
		miny = camera_position.y - renderer.ScreenHeight / 2;
		maxy = camera_position.y + renderer.ScreenHeight / 2;
	} else if (r == 2) {
		minx = camera_position.x - renderer.ScreenWidth / 2 - 50.0f;
		maxx = camera_position.x + renderer.ScreenWidth / 2 + 50.0f;
		miny = camera_position.y + renderer.ScreenHeight / 2;
		maxy = miny + 50.0f;
	} else {
		minx = camera_position.x - renderer.ScreenWidth / 2 - 50.0f;
		maxx = camera_position.x + renderer.ScreenWidth / 2 + 50.0f;
		miny = camera_position.y - renderer.ScreenHeight / 2 - 50.0f;
		maxy = miny + 50.0f;
	}
	float x = minx + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxx-minx)));
	float y = miny + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maxy-miny)));
	int type = rand() % 2;
	int col = rand() % 2;
	Renderer::BulletColor c;
	if (col == 0) c = Renderer::BulletColor::Blue;
	if (col == 1) c = Renderer::BulletColor::Red;
	if (type == 0) {
		Renderer::Enemy e = renderer.new_enemy(glm::vec2(x, y), 0, Renderer::EnemyType::Hunter, c);
		enemy_wrapper ewrap;
		ewrap.e = e;
		enemies.push_back(ewrap);
	} else if (type == 1) {
		Renderer::Enemy e = renderer.new_enemy(glm::vec2(x, y), 0, Renderer::EnemyType::Soldier, c);
		enemy_wrapper ewrap;
		ewrap.e = e;
		enemies.push_back(ewrap);
	}
}