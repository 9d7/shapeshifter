#include "Model.hpp"
#include "Animation.hpp"
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

	enemy = std::shared_ptr<Enemy>(enemies->acquire("soldier", Bullet::Blue, glm::vec2(0, 0)));
}

void Model::update(float elapsed) {


	enemies->update(elapsed, player_position);
	bullets->update(elapsed);
	for (BulletManager::iterator b_it = bullets->begin(); b_it != bullets->end();) {

		if ((*b_it)->get_age() > 1.0f) {
			b_it = bullets->erase(b_it);
		} else {
			b_it++;
		}

	}

	player_velocity += player_force * elapsed;
	player_force = glm::vec2(0.0f, 0.0f);

	if (glm::length(player_velocity) > MAX_VELOCITY) {
		player_velocity = glm::normalize(player_velocity) * MAX_VELOCITY;
	}

	player_position += player_velocity * elapsed;
	// make friction timestep-independent
	player_velocity *= glm::pow(FRICTION, 60 * elapsed);

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

	bullets->acquire(view->sprites, color, player_position, direction);
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
