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
	Bullet::Color     color_
) :
	pos(pos_),
	color(color_),
	spr_mgr(spr_mgr_),
	shooter(in.attack_list, spr_mgr_, blt_mgr, in.aim_mode, in.shoot_delay),
	moveStyle(in.move),
	health(in.health)
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
	//printf("b4 update %d\n", health);
	glm::vec2 to_player = player_pos - pos;
	if (glm::length(to_player) > 0.0f && moveStyle != Deadturret) {
		spr->set_rotation(glm::atan(to_player.y, to_player.x));
	}

	move(elapsed, player_pos);
	spr->set_position(pos);
	//printf("atfer update %d\n", health);
}

glm::vec2 Enemy::position() const {
	return pos;
}

glm::vec2 Enemy::size() const {
	const Animation::Static s = *(spr->frame().lock());
	return glm::vec2(s.w, s.h);
}

void Enemy::move(float elapsed, const glm::vec2 &player_position) {
	glm::vec2 diff = player_position - pos;
	glm::vec2 move_enemy = glm::normalize(player_position - pos);
	if (moveStyle == Soldier) {
		float dist = sqrt(dot(diff, diff));
		move_enemy = move_enemy * 110.0f * elapsed;
		if (dist > 150.0f) {
			pos = pos + move_enemy;
		} else if (dist < 70.0f) {
			pos = pos - move_enemy;
		} else {
			shooter.update(elapsed, color, player_position, pos);
		}
	} else if (moveStyle == Hunter) {
		move_enemy = move_enemy * 150.0f * elapsed;
		pos = pos + move_enemy;
	} else if (moveStyle == Turret) {
		// don't move
		shooter.update(elapsed, color, player_position, pos);
	} else if (moveStyle == Ninja) {
		auto perp = glm::vec2(move_enemy.y, -move_enemy.x);
		if (!strafeDir) perp = glm::vec2(-perp.x, -perp.y);
		perp = perp * 200.0f * elapsed;
		strafe += elapsed;
		if (strafe > 0.4f) {
			strafe = 0.0f;
			strafeDir = !strafeDir;
		}
		move_enemy = move_enemy * 140.0f * elapsed;
		pos = pos + perp;
		float dist = sqrt(dot(diff, diff));
		if (dist > 100.0f) {
			pos = pos + move_enemy;
		} else if (dist < 30.0f) {
			pos = pos - move_enemy;
		} else {
			shooter.update(elapsed, color, player_position, pos);
		}
	} else if (moveStyle == Wizard) {
		float dist = sqrt(dot(diff, diff));
		move_enemy = move_enemy * 110.0f * elapsed;
		tp += elapsed;
		if (dist > 90.0f) {
			pos = pos + move_enemy;
		} else {
			if (tp > 2.0f) {
				float angle = (rand() % 361) * (3.1416f / 180.0f);
				float x = cos(angle)*75.0f;
				float y = sin(angle)*75.0f;
				pos.x = player_position.x + x;
				pos.y = player_position.y + y;
				tp = 0.0f;
				tped = true;
			}
			if (tped) shooter.update(elapsed, color, player_position, pos);
			if (tp > 0.6f) tped = false;
		}
	} else if (moveStyle == Shifter) {
		shift += elapsed;
		if (shift > 2.0f) {
			Animation::Animation anim;
			if (color == Bullet::Color::Red) {
				anim = Animation::find_static("shifter_blue");
				color = Bullet::Color::Blue;
			}
			else if (color == Bullet::Color::Blue) {
				anim = Animation::find_static("shifter_red");
				color = Bullet::Color::Red;
			}
			spr = spr_mgr->from_anim(anim, true);
			shift = 0.0f;
		}
		float dist = sqrt(dot(diff, diff));
		move_enemy = move_enemy * 110.0f * elapsed;
		if (dist > 140.0f) {
			pos = pos + move_enemy;
		} else if (dist < 60.0f) {
			pos = pos - move_enemy;
		} else {
			shooter.update(elapsed, color, player_position, pos);
		}
	} else if (moveStyle == Shield) {
		float dist = sqrt(dot(diff, diff));
		move_enemy = move_enemy * 120.0f * elapsed;
		if (dist > 20.0f) {
			pos = pos + move_enemy;
		}
		if (dist < 80.0f) {
			shooter.update(elapsed, color, player_position, pos);
		}
	} else if (moveStyle == Repairman) {
		auto perp = glm::vec2(move_enemy.y, -move_enemy.x);
		if (!strafeDir) perp = glm::vec2(-perp.x, -perp.y);
		perp = perp * 200.0f * elapsed;
		strafe += elapsed;
		if (strafe > 0.2f) {
			strafe = 0.0f;
			strafeDir = !strafeDir;
		}
		move_enemy = move_enemy * 150.0f * elapsed;
		pos = pos + perp;
		float dist = sqrt(dot(diff, diff));
		if (dist < 70.0f) {
			pos = pos - move_enemy;
		} 
		shooter.update(elapsed, color, player_position, pos);
	}
}

int Enemy::take_damage(int damage) {
	health -= damage;
	if (health <= 0 && moveStyle == Turret) {
		Animation::Animation anim;
		if (color == Bullet::Color::Red) anim = Animation::find_static("deadturret_red");
		if (color == Bullet::Color::Blue) anim = Animation::find_static("deadturret_blue");
		spr = spr_mgr->from_anim(anim, true);
		health = 1000;
		moveStyle = Deadturret;
	}
	return health;
}
