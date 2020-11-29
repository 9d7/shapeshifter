#include "BulletManager.hpp"
#include "Animation.hpp"
#include "Bullet.hpp"
#include "Sprite.hpp"
#include "SpriteManager.hpp"
#include <cstdio>
#include <memory>
#include <stdexcept>

BulletManager::BulletManager() {

	red_bullet    = Animation::find_dynamic("red_bullet");
	blue_bullet   = Animation::find_dynamic("blue_bullet");
	purple_bullet = Animation::find_dynamic("purple_bullet");
	dead_bullet   = Animation::find_dynamic("bullet_death");

}

BulletManager::~BulletManager() {}

BulletManager::iterator BulletManager::erase(const_iterator position) {

	dead_bullets.push_front((*position)->sprite);
	dead_bullets.front()->set_position((*position)->position);

	auto db_it = dead_bullets.begin();
	dead_bullets.front()->override_animation(
		dead_bullet,
		false,
		[this, db_it](){this->dead_bullets.erase(db_it);}
	);

	return bullets.erase(position);
}

std::shared_ptr<Bullet> BulletManager::acquire(std::shared_ptr<SpriteManager> sprite_manager, Bullet::Color color, const glm::vec2 &position, const glm::vec2 &velocity) {

	Animation::Animation anim;
	switch (color) {
		case Bullet::Red:    anim = red_bullet; break;
		case Bullet::Blue:   anim = blue_bullet; break;
		case Bullet::Purple: anim = purple_bullet; break;
		default: throw std::runtime_error("Bullet color unimplemented");
	}

	std::shared_ptr<Sprite> sprite = sprite_manager->from_anim(anim, true);
	sprite->set_position(position);
	sprite->set_rotation(0.0f);


	std::shared_ptr<Bullet> ret = std::make_shared<Bullet>(position, velocity, color, sprite);

	bullets.emplace_back(ret);
	return ret;


}

void BulletManager::update(float elapsed) {
	for (std::shared_ptr<Bullet> b : bullets) {
		b->update(elapsed);
	}
}
