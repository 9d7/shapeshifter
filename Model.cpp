#include "Model.hpp"
#include "Animation.hpp"
#include "glm/geometric.hpp"
#include <memory>

Model::Model(std::shared_ptr<View> view_) : view(view_) {

	// sprite allocation is done like this because the find_static and find_dynamic functions
	// are relatively expensive--if you're constantly switching between certain animations
	// (like player_blue, player_red, and all the bullets, for example)
	// it might be a good idea to store the animations and then just run Animation::find_*
	// in the constructor for the class
	// - Eric

	player_sprite = view->sprites.from_anim(Animation::find_static("player_blue"), false);
}

void Model::update(float elapsed) {

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
	view->update_camera(camera_position);

	update_view();
}

void Model::update_view() {

	player_sprite->set_position(player_position);
	player_sprite->set_rotation(0.0f);
}

void Model::control_player(const glm::vec2 &direction) {
	player_force += direction * MOVE_FORCE;
}
