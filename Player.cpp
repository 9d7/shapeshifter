#include "Player.hpp"
#include <glm/gtc/constants.hpp>

Player::Player(std::shared_ptr<View> view)
{
	player_red = Animation::find_static("player_red");
	player_blue = Animation::find_static("player_blue");
	sprite = view->sprites->from_anim(player_blue, false);
}

void Player::update(float elapsed) {
	
	velocity += force * elapsed;
	force = glm::vec2(0.0f, 0.0f);

	if (glm::length(velocity) > MAX_VELOCITY) {
		velocity = glm::normalize(velocity) * MAX_VELOCITY;
	}
	//printf("V: %f MAXV: %f\n", glm::length(velocity), MAX_VELOCITY); // TODO remove
	position += velocity * elapsed;

	// Friction
	if (glm::length(velocity) > 0.0f) { // friction force
		friction = glm::normalize(velocity) * -1.0f * FRICTION_FORCE * elapsed;
		if (glm::length(velocity) < glm::length(friction)) velocity = glm::vec2(0.0f, 0.0f);
		else velocity += friction;
	}
}

void Player::update_sprite() {
	sprite->set_position(position);
}

void Player::move(const glm::vec2& direction) {
	force += direction * MOVE_FORCE;
}

glm::vec2 Player::shoot(const glm::vec2 mouse_world_position) {
	glm::vec2 direction(1.0f, 0.0f);
	glm::vec2 player_to_mouse = mouse_world_position - position;
	if (player_to_mouse != glm::vec2(0.0f, 0.0f)) {
		direction = glm::normalize(player_to_mouse);
	}
	direction *= BULLET_SPEED;
	return direction;
	
}

void Player::reset_player(){
	position = glm::vec2(0.0f, 0.0f);
	velocity = glm::vec2(0.0f, 0.0f);
	rotation = 0.0f;
	sprite->set_rotation(0.0f);
	sprite->set_position(position);
}

void Player::toggle_aim_assist() {
	aim_assist_active = !aim_assist_active;
}

void Player::set_color(Bullet::Color new_color) {
	color = new_color;
	Animation::Animation anim = color == Bullet::Color::Blue ? player_blue : player_red;
	sprite->override_animation(anim, false);
}

void Player::set_rotation(float new_rotation) {
	rotation = new_rotation;
	sprite->set_rotation(rotation);
}

void Player::set_rotation(float new_rotation, float elapsed) {
	if (rotation_limit < 0.0f) return set_rotation(new_rotation);
	while (abs(rotation) > glm::pi<float>()) rotation -= 2 * glm::pi<float>() * (rotation < 0.0f ? -1.0f : 1.0f);

	float delta = new_rotation - rotation;
	float sign = delta < 0.0f ? -1.0f : 1.0f;
	if (rotation < 0.0f != new_rotation < 0.0f) {
		delta -= 2 * glm::pi<float>() * sign;
		if (abs(rotation) + abs(new_rotation) > glm::pi<float>()) sign *= -1.0f;
	}
	
	float max_delta = rotation_limit * elapsed;
	// printf("rot %f | new %f | delta %f | maxlim %f | sign %f | sum %f\n", rotation, new_rotation, delta, max_delta, sign, abs(rotation) + abs(new_rotation)); //TODO remove

	if (abs(delta) < max_delta) {
		rotation = new_rotation;
	}
	else {
		rotation += max_delta * sign;
	}
	sprite->set_rotation(rotation);
}

void Player::set_position(glm::vec2 new_position) {
	position = new_position;
	sprite->set_position(new_position);
}

void Player::set_velocity(glm::vec2 new_velocity){
	velocity = new_velocity;
}

void Player::set_sprite(std::shared_ptr<Sprite> new_sprite){
	sprite = new_sprite;
}

void Player::set_aim_assist_angle(float new_assist_angle) {
	aim_assist_angle = new_assist_angle;
}

void Player::set_rotation_limit(float new_rotation_limit) {
	rotation_limit = new_rotation_limit;
}

glm::vec2 Player::get_position() const {
	return position;
}

glm::vec2 Player::get_velocity() const {
	return velocity;
}

std::shared_ptr<Sprite> Player::get_sprite() {
	return sprite;
}

Bullet::Color Player::get_color() const {
	return color;
}

float Player::get_rotation() {
	return rotation;
}
