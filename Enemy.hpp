#pragma once

/*
 * This defines an Enemy class. You shouldn't instantiate an enemy here--
 * go through an EnemyManager, which will keep iterators to each enemy,
 * and allow you to loop through them based on type.
 */

#include "Animation.hpp"
#include "BulletManager.hpp"
#include "BulletSequencer.hpp"
#include "Numeric.hpp"
#include "Sprite.hpp"
#include "SpriteManager.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include "EnemyData.hpp"
#include "BulletShooter.hpp"

class Enemy {
	public:

		~Enemy(){};

		void update(float elapsed, const glm::vec2 &player_pos);

		static Enemy *acquire(
			const std::string &name,
			Bullet::Color color,
			const glm::vec2 &pos
		);

		// this is my bad--this wouldn't have to be the case if spr_mgr
		// and blt_mgr were namespaces, but it was scared of doing that for
		// dumb reasons. likely this deserves a refactor, but that'll
		// never happen in the time we have left :) - eric
		static void set_managers(
			std::shared_ptr<SpriteManager> spr_mgr,
			std::shared_ptr<BulletManager> blt_mgr
		);

	protected:

		static std::shared_ptr<SpriteManager> spr_mgr;
		static std::shared_ptr<BulletManager> blt_mgr;

		// this struct holds all the inputs that are specific to an
		// enemy type. that way, it can be cached and we don't have
		// to look it up all the time
		struct EnemyInputs {
			Animation::Animation            anim_red;
			Animation::Animation            anim_blue;
			const BulletShooter::AttackList &attack_list;
			BulletShooter::AimMode          aim_mode;
			Numeric                         &shoot_delay;
		};
		static std::unordered_map<std::string, EnemyInputs> enemy_inputs;

		Enemy(
			const EnemyInputs &in,
			const glm::vec2 &pos,
			Bullet::Color color
		);

		glm::vec2 pos;
		Bullet::Color color;

		std::shared_ptr<Sprite> spr;

		BulletShooter shooter;



};
