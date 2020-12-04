#include "Bullet.hpp"

glm::vec2 Bullet::get_position() {
	return position;
}

float Bullet::get_age() {
	return age;
}

glm::vec2 Bullet::get_velocity()
{
	return velocity;
}

void Bullet::update(float elapsed) {
	position += velocity * elapsed;
	sprite->set_position(position);
	age += elapsed;
}
