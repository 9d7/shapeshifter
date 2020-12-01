#include "Model.hpp"
#include "Animation.hpp"
#include "BulletManager.hpp"
#include "EnemyData.hpp"
#include "View.hpp"
#include "glm/geometric.hpp"
#include <memory>

Model::Model(std::shared_ptr<View> view_) : view(view_) {

	// sprite allocation is done like this because the find_static and find_dynamic functions
	// are relatively expensive--if you're constantly switching between certain animations
	// (like player_blue, player_red, and all the bullets, for example)
	// it might be a good idea to store the animations and then just run Animation::find_*
	// in the constructor for the class
	// - Eric
	player_red = Animation::find_static("player_red");
	player_blue = Animation::find_static("player_blue");

	player_sprite = view->sprites->from_anim(player_blue, false);
	bullets = std::make_shared<BulletManager>();

	EnemyData::num("soldier", "health");
}

void Model::update(float elapsed) {

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
	player_velocity *= FRICTION;

	// update camera to be out of dead space
	static const glm::vec2 MARGIN = glm::vec2(
			View::ScreenWidth,
			View::ScreenHeight
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
	direction += player_velocity;

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
