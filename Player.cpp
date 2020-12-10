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
	if (hit_cd >= 0) hit_cd -= elapsed;
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
	sprite->set_rotation(rotation);
}

void Player::move(const glm::vec2& direction) {
	force += direction * MOVE_FORCE;
}

glm::vec2 Player::shoot(const glm::vec2 target_position) {
	// TODO add slight variation here and a way to control it
	glm::vec2 direction(1.0f, 0.0f);
	glm::vec2 player_to_target = target_position - position;
	if (player_to_target != glm::vec2(0.0f, 0.0f)) {
		direction = glm::normalize(player_to_target);
	}
	return direction *= BULLET_SPEED;

}

void Player::reset_player(glm::vec2 reset_position, glm::vec2 reset_velocity, float reset_rotation){
	position = reset_position;
	velocity = reset_velocity;
	rotation = reset_rotation;
	set_lives(10);
	update_sprite();
	//sprite->set_rotation(0.0f);
	//sprite->set_position(position);
}

void Player::hit()
{
	if (hit_cd > 0) return;
	lives--;
	hit_cd = 0.5f;

}

void Player::bind(glm::vec2 center, glm::vec2 bounds) {
	float max_x = center.x + bounds.x;
	float max_y = center.y + bounds.y;
	float min_x = center.x - bounds.x;
	float min_y = center.y - bounds.y;
	if (position.x > max_x) position.x = max_x;
	if (position.y > max_y) position.y = max_y;
	if (position.x > min_x) position.x = min_x;
	if (position.y > min_y) position.y = min_y;
}

void Player::set_lives(int new_lives)
{
	lives = new_lives;
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
	if (rotation_lock_active) return set_rotation(rotation);
	while (glm::abs(rotation) > glm::pi<float>()) rotation -= 2 * glm::pi<float>() * (rotation < 0.0f ? -1.0f : 1.0f);


	float delta = new_rotation - rotation;
	float sign = delta < 0.0f ? -1.0f : 1.0f;
	if ((rotation < 0.0f) != (new_rotation < 0.0f)) {
		delta -= 2 * glm::pi<float>() * sign;
		if (glm::abs(rotation) + glm::abs(new_rotation) > glm::pi<float>()) sign *= -1.0f;
	}

	float max_delta = rotation_limit * elapsed;
	// printf("rot %f | new %f | delta %f | maxlim %f | sign %f | sum %f\n", rotation, new_rotation, delta, max_delta, sign, abs(rotation) + abs(new_rotation)); //TODO remove

	if (glm::abs(delta) < max_delta) {
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

void Player::set_aim_assist_mode(AssistMode new_assist_mode) {
	assist_mode = new_assist_mode;
}

void Player::set_rotation_limit(float new_rotation_limit) {
	rotation_limit = new_rotation_limit;
}

void Player::set_rotation_lock(bool new_rotation_lock) {
	rotation_lock_active = new_rotation_lock;
}

glm::vec2 Player::get_position() const {
	return position;
}

glm::vec2 Player::get_velocity() const {
	return velocity;
}

std::shared_ptr<Sprite> Player::get_sprite() const {
	return sprite;
}

Bullet::Color Player::get_color() const {
	return color;
}

float Player::get_rotation() const {
	return rotation;
}

Player::AssistMode Player::get_assist_mode() const {
	return assist_mode;
}

int Player::get_lives() const
{
	return lives;
}
