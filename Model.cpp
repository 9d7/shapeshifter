#include "Model.hpp"
#include "Animation.hpp"
#include "Bullet.hpp"
#include "BulletManager.hpp"
#include "EnemyData.hpp"
#include "EnemyManager.hpp"
#include "View.hpp"
#include "glm/geometric.hpp"
#include <memory>

Model::Model(std::shared_ptr<View> view_) : view(view_) {

	player_red = Animation::find_static("player_red");
	player_blue = Animation::find_static("player_blue");

	player_sprite = view->sprites->from_anim(player_blue, false);
	bullets = std::make_shared<BulletManager>();

	enemies = std::make_shared<EnemyManager>(view->sprites, bullets);

	enemies->acquire("soldier", Bullet::Blue, glm::vec2(0, 0));
}

void Model::update(float elapsed) {


	enemies->update(elapsed, player_position);
	bullets->update(elapsed);
	for (BulletManager::iterator b_it = bullets->begin(); b_it != bullets->end();) {

		bool should_erase = false;
		const Bullet b = **b_it;

		if ((*b_it)->get_age() > 1.0f) {
			should_erase = true;
		} else {

			if (b.from_player()) {
				for (EnemyManager::iterator e_it = enemies->begin(); e_it != enemies->end();) {
					const Enemy e = **e_it;
					float radius = (e.size().x + e.size().y) / 4.0f; // average of w and h, over 2

					if (glm::length(e.position() - b.get_position()) < radius + 4.0f) {
						// kill enemy and bullet
						e_it = enemies->erase(e_it);
						should_erase = true;
						break;
					} else {
						e_it++;
					}
				}
			} else {
				if (glm::length(player_position - b.get_position()) < 8.0f + 4.0f) {
					printf("Player hit\n");
					should_erase = true;
				}
			}

		}

		if (should_erase) {
			b_it = bullets->erase(b_it);
		} else {
			b_it++;
		}

	}

	float player_velocity_magnitude = glm::length(player_velocity); // just to save computation time, though likely unneeded tbh

	
	
	player_velocity += player_force * elapsed;
	printf("V: %f MAXV: %f ||| F: %f\n", glm::length(player_velocity), MAX_VELOCITY, glm::length(player_force)); // TODO remove
	player_force = glm::vec2(0.0f, 0.0f);

	if (player_velocity_magnitude > MAX_VELOCITY) {
		player_velocity = glm::normalize(player_velocity) * MAX_VELOCITY;
	}
	
	player_position += player_velocity * elapsed;

	bool force_based_friction = true; // TODO delete this variable and the lazy friction method
	if (player_velocity_magnitude > 0.0f && force_based_friction) { // Actual friction force
		player_friction = glm::normalize(player_velocity) * -1.0f * FRICTION_FORCE * elapsed;
		if (player_velocity_magnitude < glm::length(player_friction)) player_velocity = glm::vec2(0.0f, 0.0f);
		else player_velocity += player_friction;
	}
	else { // lazy friction
		// make friction timestep-independent
		player_velocity *= glm::pow(FRICTION, 60 * elapsed);
	}

	// update camera to be out of dead space
	static const glm::vec2 MARGIN = glm::vec2(
			View::ScreenWidth,
			View::ScreenHeight
	) * 1.0f / 4.0f;

	glm::vec2 space_from_camera_center = ideal_camera_position - player_position;

	if (space_from_camera_center.x > MARGIN.x) {
		ideal_camera_position.x = MARGIN.x + player_position.x;
	}
	if (-space_from_camera_center.x > MARGIN.x) {
		ideal_camera_position.x = player_position.x - MARGIN.x;
	}

	if (space_from_camera_center.y > MARGIN.y) {
		ideal_camera_position.y = MARGIN.y + player_position.y;
	}
	if (-space_from_camera_center.y > MARGIN.y) {
		ideal_camera_position.y = player_position.y - MARGIN.y;
	}

	camera_position += (ideal_camera_position - camera_position) * glm::pow(CAMERA_SMOOTHNESS, 60 * elapsed);
	set_mouse_position(mouse_ui_position);
	view->update_camera(camera_position);

	update_view();
}

void Model::update_view() {

	player_sprite->set_position(player_position);
	player_sprite->set_rotation(0.0f);

	glm::vec2 player_to_mouse = mouse_world_position - player_position;
	if (player_to_mouse != glm::vec2(0.0f, 0.0f)) {
		player_sprite->set_rotation(glm::atan(player_to_mouse.y, player_to_mouse.x));
		//TODO set rotation to velocity and set turret to above^
	}
}

void Model::player_move(const glm::vec2 &direction) {
	player_force += direction * MOVE_FORCE;
}

void Model::player_shoot(Bullet::Color color) {

	glm::vec2 direction (1.0f, 0.0f);
	glm::vec2 player_to_mouse = mouse_world_position - player_position;
	if (player_to_mouse != glm::vec2(0.0f, 0.0f)) {
		direction = glm::normalize(player_to_mouse);
	}
	direction *= BULLET_SPEED;

	bullets->acquire(view->sprites, color, player_position, direction, true);
}

void Model::set_mouse_position(const glm::vec2 &position) {
	mouse_ui_position = position;
	mouse_world_position = position - glm::vec2(View::ScreenWidth, View::ScreenHeight) / 2.0f + camera_position;
}

void Model::player_color(Bullet::Color color) {
	player_col = color;
	Animation::Animation anim = player_col == Bullet::Color::Blue ? player_blue : player_red;
	player_sprite->override_animation(anim, false);
}
