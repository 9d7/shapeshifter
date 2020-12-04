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

std::shared_ptr<SpriteManager> Enemy::spr_mgr = nullptr;
std::shared_ptr<BulletManager> Enemy::blt_mgr = nullptr;
std::unordered_map<std::string, Enemy::EnemyInputs> Enemy::enemy_inputs {};

void Enemy::set_managers(
	std::shared_ptr<SpriteManager> spr_mgr_,
	std::shared_ptr<BulletManager> blt_mgr_) {

	spr_mgr = spr_mgr_;
	blt_mgr = blt_mgr_;

}

Enemy *Enemy::acquire(
	const std::string &name,
	Bullet::Color color,
	const glm::vec2 &pos
) {

	if (spr_mgr == nullptr) {
		throw std::runtime_error("Enemy::acquire called before Enemy::set_managers");
	}

	auto it = enemy_inputs.find(name);
	if (it == enemy_inputs.end()) {

		Animation::Animation red_anim;
		Animation::Animation blue_anim;
		const std::string &red_anim_name = EnemyData::str(name, "red sprite");
		const std::string &blue_anim_name = EnemyData::str(name, "blue sprite");

		if (EnemyData::str(name, "sprite type") == "dynamic") {
			red_anim = Animation::find_dynamic(red_anim_name);
			blue_anim = Animation::find_dynamic(blue_anim_name);
		} else {
			red_anim = Animation::find_static(red_anim_name);
			blue_anim = Animation::find_static(blue_anim_name);
		}

		BulletShooter::AimMode aim_mode;
		if (EnemyData::str(name, "aim") == "once") {
			aim_mode = BulletShooter::AimMode::Once;
		} else {
			aim_mode = BulletShooter::AimMode::Always;
		}

		// build and insert the struct in one move
		enemy_inputs.insert(std::pair<std::string, EnemyInputs>(name,
			{
				red_anim,
				blue_anim,
				EnemyData::get_attack_list(name),
				aim_mode,
				EnemyData::num(name, "firing delay")
			}
		));
		it = enemy_inputs.find(name);
	}

	return new Enemy(it->second, pos, color);


}

Enemy::Enemy (
	const EnemyInputs &in,
	const glm::vec2   &pos_,
	Bullet::Color     color_
) :
	pos(pos_),
	color(color_),
	shooter(in.attack_list, spr_mgr, blt_mgr, in.aim_mode, in.shoot_delay)
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
	// update shooter
	shooter.update(elapsed, color, player_pos, pos);

	// face player
	glm::vec2 to_player = pos - player_pos;
	if (glm::length(to_player) > 0.0f) {
		spr->set_rotation(glm::atan(to_player.y, to_player.x));
	}

}
