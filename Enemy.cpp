#include "Enemy.hpp"
#include "Animation.hpp"
#include "BulletManager.hpp"
#include "BulletShooter.hpp"
#include "EnemyData.hpp"
#include "Model.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <chrono>
#include <stdexcept>

Enemy::Enemy (
	const EnemyInputs &in,
	std::shared_ptr<SpriteManager> spr_mgr_,
	std::shared_ptr<BulletManager> blt_mgr,
	const glm::vec2   &pos_,
	Bullet::Color     color_,
	MovementStyle 	  move_
) :
	pos(pos_),
	color(color_),
	spr_mgr(spr_mgr_),
	shooter(in.attack_list, spr_mgr_, blt_mgr, in.aim_mode, in.shoot_delay),
	moveStyle(move_)
{

	Animation::Animation anim;
	if (color_ == Bullet::Color::Red) {
		anim = in.anim_red;
	} else {
		anim = in.anim_blue;
	}

	spr = spr_mgr->from_anim(anim, true);
	spr->set_position(pos);
	spr->set_rotation(0.0f);

}

void Enemy::update(float elapsed, const glm::vec2 &player_pos) {
	
	// face player
	glm::vec2 to_player = player_pos - pos;
	if (glm::length(to_player) > 0.0f) {
		spr->set_rotation(glm::atan(to_player.y, to_player.x));
	}

	move(elapsed, player_pos);
	spr->set_position(pos);
}

glm::vec2 Enemy::position() const {
	return pos;
}

glm::vec2 Enemy::size() const {
	const Animation::Static s = *(spr->frame().lock());
	return glm::vec2(s.w, s.h);
}

void Enemy::move(float elapsed, const glm::vec2 &player_position) {
	if (moveStyle == Soldier) {
		float xdiff = pos.x - player_position.x;
		float ydiff = pos.y - player_position.y;
		float dist = sqrt(xdiff*xdiff + ydiff*ydiff);
		glm::vec2 move_enemy = glm::normalize(player_position - pos);
		move_enemy = move_enemy * 80.0f * elapsed;
		if (dist > 130.0f) {
			pos = pos + move_enemy;
		} else if (dist < 70.0f) {
			pos = pos - move_enemy;
		} else {
			// update shooter
			shooter.update(elapsed, color, player_position, pos);
		}
	} else if (moveStyle == Hunter) {
		glm::vec2 move_enemy = glm::normalize(player_position - pos);
		move_enemy = move_enemy * 150.0f * elapsed;
		pos = pos + move_enemy;
	}
}
