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
	move_style(in.move),
	pos(pos_),
	color(color_),
	spr_mgr(spr_mgr_),
	shooter(in.attack_list, spr_mgr_, blt_mgr, in.aim_mode, in.shoot_delay),
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
	glm::vec2 to_player = player_pos - pos;
	if (glm::length(to_player) > 0.0f && move_style != Deadturret) {
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
	glm::vec2 diff = player_position - pos;
	glm::vec2 move_enemy = glm::normalize(player_position - pos);
	if (move_style == Soldier) {
		float dist = sqrt(dot(diff, diff));
		move_enemy = move_enemy * 110.0f * elapsed;
		if (dist > 150.0f) {
			pos = pos + move_enemy;
		} else if (dist < 70.0f) {
			pos = pos - move_enemy;
		} else {
			shooter.update(elapsed, color, player_position, pos);
		}
	} else if (move_style == Hunter) {
		move_enemy = move_enemy * 150.0f * elapsed;
		pos = pos + move_enemy;
	} else if (move_style == Turret) {
		// don't move
		shooter.update(elapsed, color, player_position, pos);
	} else if (move_style == Ninja) {
		auto perp = glm::vec2(move_enemy.y, -move_enemy.x);
		if (!strafe_dir) perp = glm::vec2(-perp.x, -perp.y);
		perp = perp * 200.0f * elapsed;
		strafe += elapsed;
		if (strafe > 0.4f) {
			strafe = 0.0f;
			strafe_dir = !strafe_dir;
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
	} else if (move_style == Wizard) {
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
	} else if (move_style == Shifter) {
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
	} else if (move_style == Shield) {
		float dist = sqrt(dot(diff, diff));
		move_enemy = move_enemy * 120.0f * elapsed;
		if (dist > 20.0f) {
			pos = pos + move_enemy;
		}
		if (dist < 80.0f) {
			shooter.update(elapsed, color, player_position, pos);
		}
	} else if (move_style == Repairman) {
		std::shared_ptr<Enemy> closest = Model::get_closest(pos);
		glm::vec2 perp;
		strafe += elapsed;
		if (strafe > 1.0f) {
			strafe = 0.0f;
			strafe_dir = !strafe_dir;
		}
		float dist = sqrt(dot(diff, diff));
		if (dist < 70.0f) {
			move_enemy = move_enemy * 150.0f * elapsed;
			pos = pos - move_enemy;
			perp = glm::vec2(move_enemy.y, -move_enemy.x);
			perp = perp * 180.0f * elapsed;
			if (!strafe_dir) perp = glm::vec2(-perp.x, -perp.y);
			pos = pos + perp;

		} else if (dist > 75.0f && closest != nullptr) {
			glm::vec2 move_turret = glm::normalize((*closest).position() - pos);
			move_turret = move_turret * 150.0f * elapsed;
			pos = pos + move_turret;
			perp = glm::vec2(move_turret.y, -move_turret.x);
			float turret_dist = sqrt(dot((*closest).position() - pos, (*closest).position() - pos));
			if (turret_dist > 30.0f) {
				perp = perp * 180.0f * elapsed;
				if (!strafe_dir) perp = glm::vec2(-perp.x, -perp.y);
				pos = pos + perp;
			} else if (turret_dist < 10.0f) repair_turret(closest);

		} else {
			perp = glm::vec2(move_enemy.y, -move_enemy.x);
			perp = perp * 180.0f * elapsed;
			if (!strafe_dir) perp = glm::vec2(-perp.x, -perp.y);
			pos = pos + perp;
		}

		shooter.update(elapsed, color, player_position, pos);
	}
}

int Enemy::take_damage(int damage) {
	health -= damage;
	return health;
}

void Enemy::dead_turret() {
	Animation::Animation anim;
	if (color == Bullet::Color::Red) anim = Animation::find_static("deadturret_red");
	if (color == Bullet::Color::Blue) anim = Animation::find_static("deadturret_blue");
	spr = spr_mgr->from_anim(anim, true);
	health = 500;
	move_style = Deadturret;
}

void Enemy::repair_turret(std::shared_ptr<Enemy> e) {
	Animation::Animation anim;
	if (e->color ==  Bullet::Color::Red) anim = Animation::find_static("turret_red");
	if (e->color == Bullet::Color::Blue) anim = Animation::find_static("turret_blue");
	e->spr = e->spr_mgr->from_anim(anim, true);
	e->health = 5;
	e->move_style = Turret;
}
