#include "Enemy.hpp"
#include "Animation.hpp"
#include "BulletManager.hpp"
#include "Model.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include <chrono>
#include <stdexcept>

std::mt19937 Enemy::mt ((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
std::uniform_real_distribution<float> Enemy::dist (0.0f, 1.0f);

Enemy::Enemy (
	const AttackList &attack_list_,
	const glm::vec2  &pos_,
	std::shared_ptr<SpriteManager> spr_mgr_
) : attack_list(attack_list_), pos(pos_), spr_mgr(spr_mgr_) {

	spr = spr_mgr->from_anim(Animation::find_static("soldier_blue"), false);
	spr->set_position(pos);
	spr->set_rotation(0.0f);

}

void Enemy::update(float elapsed, std::shared_ptr<BulletManager> bm) {

	if (is_firing()) firing_timer += elapsed;

	while (firing_pattern.size() > 0 && firing_timer > firing_pattern.front().time) {

		Bullet::Color color = firing_pattern.front().your_color ? Bullet::Red : Bullet::Blue;
		float velocity = firing_pattern.front().velocity;
		float angle = firing_pattern.front().angle;

		glm::vec2 enemy_to_player = saved_player_pos - pos;
		if (glm::length(enemy_to_player) > 0) {
			enemy_to_player = glm::normalize(enemy_to_player);
			spr->set_rotation(glm::atan(enemy_to_player.y, enemy_to_player.x));
		}

		glm::mat2 rot (
			glm::cos(angle), glm::sin(angle),
			-glm::sin(angle), glm::cos(angle)
		);

		enemy_to_player = rot * velocity * enemy_to_player * Model::BULLET_SPEED;

		// spawn bullet
		bm->acquire(spr_mgr, color, pos, enemy_to_player);

		firing_pattern.pop_front();
	}
	if (firing_pattern.size() == 0) {
		firing_timer = 0.0f;
	}

}

bool Enemy::is_firing() {
	return firing_pattern.size() > 0;
}

void Enemy::fire(const glm::vec2 &player_pos) {

	saved_player_pos = player_pos;

	float max = attack_list.crbegin()->first;
	float choice = dist(mt) * max;

	auto it = attack_list.lower_bound(choice);
	if (it == attack_list.end()) {
		throw std::runtime_error("Firing past end of list");
	}

	firing_pattern = it->second.get_firing_pattern();

}
