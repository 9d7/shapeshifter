#include "Bullet.hpp"

glm::vec2 Bullet::get_position() const {
	return position;
}

float Bullet::get_age() const {
	return age;
}

glm::vec2 Bullet::get_velocity() const {
	return velocity;
}

bool Bullet::from_player() const {
	return shot_by_player;
}

Bullet::Color Bullet::get_color() const {
	return color;
}

void Bullet::update(float elapsed) {
	position += velocity * elapsed;
	sprite->set_position(position);
	age += elapsed;
}
