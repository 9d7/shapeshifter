#include "BulletShooter.hpp"
#include "BulletSequencer.hpp"
#include "Model.hpp"
#include "glm/geometric.hpp"
#include <chrono>
#include <stdexcept>

std::mt19937 BulletShooter::mt ((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
std::uniform_real_distribution<float> BulletShooter::dist (0.0f, 1.0f);

void BulletShooter::update(
	float elapsed,
	Bullet::Color gun_color,
	const glm::vec2 &player_pos,
	const glm::vec2 &gun_pos
) {

	if (aim_mode == Always) {
		target = player_pos;
	}

	firing_timer += elapsed;

	// retrigger after some delay
	if (firing_pattern.size() == 0 && firing_timer >= delay_concrete) {
		firing_timer -= delay_concrete;
		delay_concrete = delay();

		fire(player_pos);
	}

	while (firing_pattern.size() > 0 && firing_timer > firing_pattern.front().time) {

		BulletSequencer::ConcreteBulletInfo info = firing_pattern.front();

		Bullet::Color color;
		if (gun_color == Bullet::Blue) {
			color = info.your_color ? Bullet::Blue : Bullet::Red;
		} else {
			color = info.your_color ? Bullet::Red : Bullet::Blue;
		}

		glm::vec2 gun_to_player = target - gun_pos;
		if (glm::length(gun_to_player) > 0.0f) {
			gun_to_player = glm::normalize(gun_to_player);
		}

		float angle = info.angle;
		glm::mat2 rot (
			glm::cos(angle), glm::sin(angle),
			-glm::sin(angle), glm::cos(angle)
		);

		gun_to_player = rot * info.velocity * gun_to_player * Model::BULLET_SPEED;
		blt_mgr->acquire(spr_mgr, color, gun_pos, gun_to_player, false);

		if (firing_pattern.size() == 1) {
			firing_timer -= info.time;
		}
		firing_pattern.pop_front();
	}

}

bool BulletShooter::fire(const glm::vec2 &player_pos) {

	if (attack_list.size() == 0) {
		return false;
	}

	target = player_pos;

	float max = attack_list.crbegin()->first;
	float choice = dist(mt) * max;

	auto it = attack_list.lower_bound(choice);
	if (it == attack_list.end()) {
		throw std::runtime_error("Firing past end of list");
	}

	firing_pattern = it->second.get_firing_pattern();
	firing_timer = 0.0f;

	return true;
}

bool BulletShooter::is_firing() {
	return firing_pattern.size() > 0;
}

float BulletShooter::time_since_firing() {
	if (is_firing()) return 0.0f;
	return firing_timer;
}
