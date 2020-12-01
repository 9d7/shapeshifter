#pragma once

/*
 * This defines an Enemy class. You shouldn't instantiate an enemy here--
 * go through an EnemyManager, which will keep iterators to each enemy,
 * and allow you to loop through them based on type.
 */

#include "BulletManager.hpp"
#include "BulletSequencer.hpp"
#include "Sprite.hpp"
#include "SpriteManager.hpp"
#include <glm/glm.hpp>
#include <random>
#include <string>

class Enemy {
	public:

		~Enemy(){};

		typedef std::map<float, BulletSequencer> AttackList;

		void fire(const glm::vec2 &player_pos);
		bool is_firing();
		void update(float elapsed, std::shared_ptr<BulletManager> bw);


		friend class EnemyManager;

	protected:

		static std::mt19937 mt;
		static std::uniform_real_distribution<float> dist;

		const AttackList &attack_list;

		glm::vec2 pos;
		glm::vec2 saved_player_pos {0.0f, 0.0f};

		std::shared_ptr<SpriteManager> spr_mgr;
		std::shared_ptr<Sprite> spr;

		std::list<BulletSequencer::ConcreteBulletInfo> firing_pattern;
		float firing_timer = 0.0f;

		Enemy (
			const AttackList &attack_list_,
			const glm::vec2  &pos_,
			std::shared_ptr<SpriteManager> spr_mgr_
		);

};
