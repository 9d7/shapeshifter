#pragma once

#include <list>
#include <memory>
#include <glm/glm.hpp>
#include "Animation.hpp"
#include "Bullet.hpp"
#include "Sprite.hpp"
#include "SpriteManager.hpp"

class BulletManager {

	public:

		BulletManager();
		~BulletManager();

		typedef std::list<std::shared_ptr<Bullet>>::iterator iterator;
		typedef std::list<std::shared_ptr<Bullet>>::const_iterator const_iterator;

		iterator begin() { return bullets.begin(); }
		iterator end() { return bullets.end(); }
		iterator erase(const_iterator position);

		std::shared_ptr<Bullet> acquire(
			std::shared_ptr<SpriteManager> sprite_manager,
			Bullet::Color                  color,
			const glm::vec2                &position,
			const glm::vec2                &velocity,
			bool                           shot_by_player
		);


		void update(float elapsed);
		std::list<std::shared_ptr<Bullet>> bullets;

	protected:

		std::list<std::shared_ptr<Sprite>> dead_bullets;

		Animation::Animation blue_bullet;
		Animation::Animation red_bullet;
		Animation::Animation purple_bullet;
		Animation::Animation dead_bullet;


};
