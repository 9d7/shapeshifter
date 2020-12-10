#pragma once

#include "BulletManager.hpp"
#include "Enemy.hpp"
#include "SpriteManager.hpp"
#include <memory>
#include <string>
#include <unordered_map>
class EnemyManager {

	public:

		EnemyManager(
			std::shared_ptr<SpriteManager> spr_mgr_,
			std::shared_ptr<BulletManager> blt_mgr_
		) : spr_mgr(spr_mgr_), blt_mgr(blt_mgr_) {};

		~EnemyManager() = default;

		typedef std::list<std::shared_ptr<Enemy>> list_type;
		typedef list_type::iterator iterator;
		typedef list_type::const_iterator const_iterator;

		iterator begin() { return enemies.begin(); }
		iterator end() { return enemies.end(); }
		iterator erase(const_iterator position);

		std::shared_ptr<Enemy> acquire(
			const std::string &name,
			Bullet::Color     color,
			const glm::vec2   &pos
		);

		int get_num_enemies();

		void update(float elapsed, const glm::vec2 &pos);
		list_type enemies;

	protected:

		// stuff for caching
		std::shared_ptr<SpriteManager> spr_mgr;
		std::shared_ptr<BulletManager> blt_mgr;
		std::unordered_map<std::string, Enemy::EnemyInputs> enemy_inputs;

};
