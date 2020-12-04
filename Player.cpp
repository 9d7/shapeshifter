#include "Player.hpp"

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
	sprite->set_rotation(0.0f);
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
	sprite->set_rotation(0.0f);
	sprite->set_position(position);
}

void Player::set_color(Bullet::Color new_color) {
	color = new_color;
	Animation::Animation anim = color == Bullet::Color::Blue ? player_blue : player_red;
	sprite->override_animation(anim, false);
}

void Player::set_rotation(float new_rotation) {
	sprite->set_rotation(new_rotation);
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
