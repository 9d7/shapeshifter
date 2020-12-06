#include "Model.hpp"
#include "Animation.hpp"
#include "Bullet.hpp"
#include "BulletManager.hpp"
#include "EnemyData.hpp"
#include "EnemyManager.hpp"
#include "View.hpp"
#include "glm/geometric.hpp"
#include <memory>

Model::Model(std::shared_ptr<View> view_) : view(view_) {

	player = std::make_shared<Player>(view);

	bullets = std::make_shared<BulletManager>();

	enemies = std::make_shared<EnemyManager>(view->sprites, bullets);

	//enemies->acquire("soldier", Bullet::Blue, glm::vec2(0, 5.0f), Enemy::Soldier);
	//enemies->acquire("soldier", Bullet::Red, glm::vec2(0, -5.0f), Enemy::Soldier);

	//enemies->acquire("hunter", Bullet::Red, glm::vec2(5.0f, 0), Enemy::Hunter);

	enemies->acquire("wizard", Bullet::Red, glm::vec2(10.0f, 0));
	enemies->acquire("ninja", Bullet::Blue, glm::vec2(-10.0f, 0));

	view->ui->set_health(10);
}

void Model::update(float elapsed) {


	player->update(elapsed);
	enemies->update(elapsed, player->get_position());
	static size_t score = 0;
	static size_t lives = 10;

	bullets->update(elapsed);


	for (BulletManager::iterator b_it = bullets->begin(); b_it != bullets->end();) {

		bool should_erase = false;
		const Bullet b = **b_it;

		if ((*b_it)->get_age() > 1.0f) {
			should_erase = true;
		} else {

			if (b.from_player()) {
				for (EnemyManager::iterator e_it = enemies->begin(); e_it != enemies->end();) {
					const Enemy e = **e_it;
					float radius = (e.size().x + e.size().y) / 4.0f; // average of w and h, over 2

					if (glm::length(e.position() - b.get_position()) < radius + 4.0f) {
						// kill enemy and bullet

						// e_it = enemies->erase(e_it);
						score += 100;
						view->ui->set_score(score);

						should_erase = true;
						break;
					} else {
						e_it++;
					}
				}
			} else {
				if (glm::length(player->get_position() - b.get_position()) < 8.0f + 4.0f) {
					if (lives > 0 && b.get_color() != player->get_color()) lives--;
					view->ui->set_health(lives);

					should_erase = true;
				}
			}

		}

		if (should_erase) {
			b_it = bullets->erase(b_it);
		} else {
			b_it++;
		}

	}
	

	// update camera to be out of dead space
	static const glm::vec2 MARGIN = glm::vec2(
			View::FieldWidth,
			View::FieldHeight
	) * 1.0f / 4.0f;

	glm::vec2 space_from_camera_center = ideal_camera_position - player->get_position();

	if (space_from_camera_center.x > MARGIN.x) {
		ideal_camera_position.x = MARGIN.x + player->get_position().x;
	}
	if (-space_from_camera_center.x > MARGIN.x) {
		ideal_camera_position.x = player->get_position().x - MARGIN.x;
	}

	if (space_from_camera_center.y > MARGIN.y) {
		ideal_camera_position.y = MARGIN.y + player->get_position().y;
	}
	if (-space_from_camera_center.y > MARGIN.y) {
		ideal_camera_position.y = player->get_position().y - MARGIN.y;
	}

	camera_position += (ideal_camera_position - camera_position) * glm::pow(CAMERA_SMOOTHNESS, 60 * elapsed);
	set_mouse_position(mouse_ui_position);
	view->update_camera(camera_position);

	update_view(elapsed);
}

void Model::update_view(float elapsed) {

	player->update_sprite();

	glm::vec2 player_to_mouse = mouse_world_position - player->get_position();
	if (player_to_mouse != glm::vec2(0.0f, 0.0f)) {
		player->set_rotation(glm::atan(player_to_mouse.y, player_to_mouse.x), elapsed);
		//TODO set rotation to velocity and set turret to above^ when multilayered sprites are in
	}
}

void Model::player_shoot() {
	glm::vec2 shot_vector = player->shoot(mouse_world_position);
	bullets->acquire(view->sprites, player->get_color(), player->get_position(), shot_vector, true);
}

void Model::player_move(glm::vec2 direction) {
	player->move(direction);
}

void Model::set_player_color(Bullet::Color color) {
	player->set_color(color);
	view->ui->set_health_color(color);
}

void Model::set_mouse_position(const glm::vec2 &position) {
	mouse_ui_position = position;
	mouse_world_position = position - glm::vec2(View::FieldWidth, View::FieldHeight) / 2.0f + camera_position;
	view->ui->set_cursor(position);
}

float Model::get_bullet_speed() const {
	return Player::BULLET_SPEED;
}
