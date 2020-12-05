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
		void move(float elapsed, const glm::vec2 &player_position);

		glm::vec2 size() const;
		glm::vec2 position() const;

		friend class EnemyManager;

		enum MovementStyle
		{
			Hunter,
			Soldier
		};


	protected:

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

		Enemy(
			const EnemyInputs &in,
			std::shared_ptr<SpriteManager> spr_mgr_,
			std::shared_ptr<BulletManager> blt_mgr,
			const glm::vec2 &pos,
			Bullet::Color color,
			MovementStyle move
		);

		glm::vec2 pos;
		Bullet::Color color;
		std::shared_ptr<SpriteManager> spr_mgr;

		std::shared_ptr<Sprite> spr;

		BulletShooter shooter;

		MovementStyle moveStyle;

};
