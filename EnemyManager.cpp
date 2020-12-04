#include "EnemyManager.hpp"
#include <stdexcept>

EnemyManager::iterator EnemyManager::erase(const_iterator position) {
	return enemies.erase(position);
}

std::shared_ptr<Enemy> EnemyManager::acquire(
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
		enemy_inputs.insert(std::pair<std::string, Enemy::EnemyInputs>(name,
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

	enemies.emplace_back(new Enemy(it->second, spr_mgr, blt_mgr, pos, color));
	return enemies.back();

}

void EnemyManager::update(float elapsed, const glm::vec2 &player_pos) {
	for (std::shared_ptr<Enemy> &e : enemies) {
		e->update(elapsed, player_pos);
	}
}
