#pragma once

#include "BulletManager.hpp"
#include "BulletSequencer.hpp"
#include "SpriteManager.hpp"
#include <memory>
#include <random>
class BulletShooter {

	public:
		typedef std::map<float, BulletSequencer> AttackList;

		enum AimMode {
			Once,  // aims once at beginning of sequence, fires at that point
			Always // aims every time a bullet is fired
			// feel free to add more (ex: limit rotation speed of enemy?)
		};

		bool fire(const glm::vec2 &player_pos);
		bool is_firing();

		// if the BulletShooter is not firing, this holds the amount of
		// time since it finished firing
		float time_since_firing();
		void update(
			float elapsed,
			Bullet::Color gun_color,
			const glm::vec2 &player_pos,
			const glm::vec2 &gun_pos
		);

		BulletShooter(
			const AttackList               &attack_list_,
			std::shared_ptr<SpriteManager> spr_mgr_,
			std::shared_ptr<BulletManager> blt_mgr_,
			AimMode                        aim_mode_,
			Numeric                        &delay_
		) :
			attack_list(attack_list_),
			spr_mgr(spr_mgr_),
			blt_mgr(blt_mgr_),
			aim_mode(aim_mode_),
			delay(delay_)
		{ delay_concrete = delay(); }

		~BulletShooter() = default;

	protected:

		glm::vec2 target;

		static std::mt19937 mt;
		static std::uniform_real_distribution<float> dist;

		const AttackList &attack_list;
		std::shared_ptr<SpriteManager> spr_mgr;
		std::shared_ptr<BulletManager> blt_mgr;
		AimMode aim_mode;
		Numeric &delay;

		float delay_concrete;

		std::list<BulletSequencer::ConcreteBulletInfo> firing_pattern;
		float firing_timer = 0.0f;


};
