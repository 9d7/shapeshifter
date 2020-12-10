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
		int take_damage(int damage);
		void repair_turret(std::shared_ptr<Enemy> e);
		void dead_turret();
		void boss_move(float elapsed, const glm::vec2 &player_position);

		glm::vec2 size() const;
		glm::vec2 position() const;
		Bullet::Color get_color() const;

		friend class EnemyManager;

		enum MovementStyle
		{
			Hunter,
			Soldier,
			Turret,
			Ninja,
			Wizard,
			Shifter,
			Shield,
			Deadturret,
			Repairman,
			Boss
		};

		MovementStyle move_style;

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
			MovementStyle					move;
			int					       		health;
		};

		Enemy(
			const EnemyInputs &in,
			std::shared_ptr<SpriteManager> spr_mgr_,
			std::shared_ptr<BulletManager> blt_mgr,
			const glm::vec2 &pos,
			Bullet::Color color
		);

		glm::vec2 pos;
		Bullet::Color color;
		std::shared_ptr<SpriteManager> spr_mgr;

		// maybe make these part of yaml
		float strafe = 0.0f;
		glm::vec2 boss_dir;
		float update_dir = 1.0f;
		bool strafe_dir = rand() % 2;
		float tp = 3.0f;
		bool tped = false;
		float shift = 0.0f;

		float speed;

		std::shared_ptr<Sprite> spr;

		BulletShooter shooter;

		int health;
};
