#include "Bullet.hpp"

glm::vec2 Bullet::get_position() const {
	return position;
}

float Bullet::get_age() const {
	return age;
}

bool Bullet::from_player() const {
	return shot_by_player;
}

void Bullet::update(float elapsed) {
	position += velocity * elapsed;
	sprite->set_position(position);
	age += elapsed;
}
