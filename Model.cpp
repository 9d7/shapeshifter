#include "Model.hpp"
#include "Animation.hpp"
#include "Bullet.hpp"
#include "BulletManager.hpp"
#include "EnemyData.hpp"
#include "EnemyManager.hpp"
#include "View.hpp"
#include "Load.hpp"
#include "glm/geometric.hpp"
#include <memory>
#include <glm/gtx/rotate_vector.hpp>
#include "MenuMode.hpp"

std::shared_ptr<EnemyManager> Model::enemies;

Load< Sound::Sample > enemy_die(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("enemy_die.wav"));
});

Load< Sound::Sample > player_shoot_sound(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("shoot.wav"));
});

Model::Model(std::shared_ptr<View> view_) : view(view_) {

	player = std::make_shared<Player>(view);

	bullets = std::make_shared<BulletManager>();

	Model::enemies = std::make_shared<EnemyManager>(view->sprites, bullets);

	level = std::make_shared<Level>();

	//enemies->acquire("soldier", Bullet::Blue, glm::vec2(0, 5.0f), Enemy::Soldier);
	//enemies->acquire("soldier", Bullet::Red, glm::vec2(0, -5.0f), Enemy::Soldier);

	//enemies->acquire("hunter", Bullet::Red, glm::vec2(5.0f, 0), Enemy::Hunter);
	//level->get_wave_enemies("level one", 0, 0);
	//level1();
	//enemies->acquire("shield", Bullet::Red, glm::vec2(10.0f, 0));
	//enemies->acquire("ninja", Bullet::Blue, glm::vec2(-20.0f, 0));
	//enemies->acquire("wizard", Bullet::Blue, glm::vec2(0.0f, 10.0f));
	//enemies->acquire("turret", Bullet::Blue, glm::vec2(0, -10.0f));
	//enemies->acquire("turret", Bullet::Red, glm::vec2(0, 10.0f));
	//enemies->acquire("repairman", Bullet::Blue, glm::vec2(10, -10.0f));
	//enemies->acquire("repairman", Bullet::Red, glm::vec2(15, -10.0f));
	//enemies->acquire("repairman", Bullet::Red, glm::vec2(-10, -15.0f));
	//enemies->acquire("boss", Bullet::Blue, glm::vec2(-15, -10.0f));

	view->ui->set_health(10);
}

void Model::update(float elapsed) {
	if (player->get_lives() <= 0) {
		//Mode::set_current(std::make_shared< MenuMode >(MenuMode::Message::None));
		player->reset_player();
		view->ui->set_health(10);
		view->ui->set_score(0);
	}

	player->update(elapsed);
	enemies->update(elapsed, player->get_position());
	static size_t score = 0;

	bullets->update(elapsed);


	for (BulletManager::iterator b_it = bullets->begin(); b_it != bullets->end();) {

		bool should_erase = false;
		const Bullet b = **b_it;
		turrets_dead = 0;
		if ((*b_it)->get_age() > 1.0f) {
			should_erase = true;
		} else {

			if (b.from_player()) {
				for (EnemyManager::iterator e_it = enemies->begin(); e_it != enemies->end();) {
					const Enemy e = **e_it;
					float radius = (e.size().x + e.size().y) / 4.0f; // average of w and h, over 2
					if (e.move_style == Enemy::MovementStyle::Turret) turrets_dead = false;
					if (e.move_style == Enemy::MovementStyle::Deadturret) turrets_dead += 1;
					if (glm::length(e.position() - b.get_position()) < radius + 4.0f) {
						// kill enemy and bullet
						if (e.get_color() != b.get_color() || e.move_style == Enemy::MovementStyle::Boss) {
							int health = (**(e_it)).take_damage(1);
							if (health == 0) {
								Sound::play(*enemy_die, 1.0f, 0.0f);
								if ((**(e_it)).move_style == Enemy::MovementStyle::Turret) (**(e_it)).dead_turret();
								else e_it = enemies->erase(e_it);
								
							}
							score += 100;
						}
						view->ui->set_score(score);

						should_erase = true;
						break;
					} else {
						e_it++;
					}
				}
			} else {
				if (glm::length(player->get_position() - b.get_position()) < 8.0f + 4.0f) {
					if (player->get_lives() > 0 && b.get_color() != player->get_color()) player->hit();
					view->ui->set_health(player->get_lives());
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

	if (enemies->enemies.size() == 0 || (turrets_dead == enemies->enemies.size())) {
		enemies->enemies.clear();
		lnum += 1;
		if (lnum == 2) level2();
		if (lnum == 3) level3();
		if (lnum == 4) level4();
		if (lnum == 5) level5();
		if (lnum == 6) level6();
		if (lnum == 7) level7();
		if (lnum == 8) level8();
		if (lnum == 9) level9();
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

	//TODO set player rotation to velocity and set turret to mouse pos when multilayered sprites are in
	// Mouse position based rotation
	glm::vec2 player_to_mouse = mouse_world_position - player->get_position();
	if (player_to_mouse != glm::vec2(0.0f, 0.0f)) {
		//player->set_rotation(glm::atan(player_to_mouse.y, player_to_mouse.x), elapsed);
	}
	// Velocity based rotation
	if (glm::length(player->get_velocity()) != 0.0f) {
		player->set_rotation(glm::atan(player->get_velocity().y, player->get_velocity().x), elapsed);
	}
}

void Model::player_shoot() {
	Sound::play(*player_shoot_sound, 1.0f, 0.0f);
	glm::vec2 shot_target = mouse_world_position;

	if (player->get_assist_mode() != Player::AssistMode::Off) {
		std::shared_ptr<Enemy> target = nullptr;
		for (EnemyManager::iterator e_it = enemies->begin(); e_it != enemies->end(); e_it++) {
			const Enemy e = **e_it;
			if (true) { // TODO remove this statement, testing only
				target = *e_it;
			}
			switch (player->get_assist_mode()) {
				case Player::AssistMode::Mouse:    // Find target closest to mouse position
					break;
				case Player::AssistMode::Closest:  // Find target closest to player position
					break;
				case Player::AssistMode::Furthest: // Find target furthest from player position
					break;
				case Player::AssistMode::Homing:   // Find target closest to mouse position and home on mouse position (or target?)
					break;
				default: break;
			}
		}
		if (target != nullptr) {
			shot_target = target->position();
		}
	}

	glm::vec2 shot_vector = player->shoot(shot_target);

	bullets->acquire(view->sprites, player->get_color(), player->get_position(), shot_vector, true);
}

void Model::player_move(glm::vec2 direction) {
	player->move(direction);
}

void Model::set_player_color(Bullet::Color color) {
	player->set_color(color);
	view->ui->set_health_color(color);
}

void Model::lock_rotation(bool rotation_lock) {
	player->set_rotation_lock(rotation_lock);
}

void Model::set_mouse_position(const glm::vec2 &position) {
	mouse_ui_position = position;
	mouse_world_position = position - glm::vec2(View::FieldWidth, View::FieldHeight) / 2.0f + camera_position;
	view->ui->set_cursor(position);
}

float Model::get_bullet_speed() const {
	return Player::BULLET_SPEED;
}

std::shared_ptr<Enemy> Model::get_closest(glm::vec2 pos) {
	std::shared_ptr<Enemy> closest = nullptr;
	float dist = 3.40282e+038f;
	for (EnemyManager::iterator e_it = enemies->begin(); e_it != enemies->end(); e_it++) {
		if ((*e_it)->move_style == Enemy::MovementStyle::Deadturret) {
			glm::vec2 diff = (*e_it)->position() - pos;
			float new_dist = sqrt(dot(diff, diff));
			if (new_dist < dist) {
				dist = new_dist;
				closest = (*(e_it));
			}
		}
	}
	return closest;
}

void Model::level1() {
	enemies->acquire("soldier", Bullet::Red, glm::vec2(20, 40.0f));
	enemies->acquire("soldier", Bullet::Red, glm::vec2(0, 40.0f));
	enemies->acquire("soldier", Bullet::Blue, glm::vec2(-20, 40.0f));
}

void Model::level2() {
	(*(player)).set_position(glm::vec2(0,0));
	enemies->acquire("turret", Bullet::Red, glm::vec2(20, 40.0f));
	enemies->acquire("turret", Bullet::Blue, glm::vec2(-20, 40.0f));
	enemies->acquire("soldier", Bullet::Blue, glm::vec2(20, -40.0f));
	enemies->acquire("soldier", Bullet::Red, glm::vec2(-20, 40.0f));
}



void Model::level3() {
	(*(player)).set_position(glm::vec2(0,0));
	enemies->acquire("soldier", Bullet::Red, glm::vec2(40, 40.0f));
	enemies->acquire("ninja", Bullet::Blue, glm::vec2(-40, 40.0f));
}


void Model::level4() {
	(*(player)).set_position(glm::vec2(0,0));
	enemies->acquire("soldier", Bullet::Red, glm::vec2(40, 40.0f));
	enemies->acquire("wizard", Bullet::Blue, glm::vec2(-40, 40.0f));
	enemies->acquire("soldier", Bullet::Red, glm::vec2(40, -40.0f));
}


void Model::level5() {
	(*(player)).set_position(glm::vec2(0,0));
	enemies->acquire("turret", Bullet::Red, glm::vec2(40, 40.0f));
	enemies->acquire("turret", Bullet::Blue, glm::vec2(-40, 40.0f));
	enemies->acquire("repairman", Bullet::Blue, glm::vec2(40, -40.0f));
	enemies->acquire("repairman", Bullet::Red, glm::vec2(-40, -40.0f));
}


void Model::level6() {
	(*(player)).set_position(glm::vec2(0,0));
	enemies->acquire("shifter", Bullet::Red, glm::vec2(40, 40.0f));
	enemies->acquire("shifter", Bullet::Blue, glm::vec2(-40, 40.0f));
	enemies->acquire("shifter", Bullet::Blue, glm::vec2(40, -40.0f));
	enemies->acquire("shifter", Bullet::Red, glm::vec2(-40, -40.0f));
}

void Model::level7() {
	(*(player)).set_position(glm::vec2(0,0));
	enemies->acquire("shifter", Bullet::Red, glm::vec2(40, 40.0f));
	enemies->acquire("shifter", Bullet::Blue, glm::vec2(-40, 40.0f));
	enemies->acquire("shield", Bullet::Blue, glm::vec2(40, 20.0f));
	enemies->acquire("shield", Bullet::Red, glm::vec2(-40, 20.0f));
}

void Model::level8() {
	(*(player)).set_position(glm::vec2(0,0));
	enemies->acquire("turret", Bullet::Red, glm::vec2(40, 50.0f));
	enemies->acquire("repairman", Bullet::Blue, glm::vec2(40, -40.0f));
	enemies->acquire("wizard", Bullet::Red, glm::vec2(40, 40.0f));
	enemies->acquire("ninja", Bullet::Blue, glm::vec2(-40, 40.0f));
	enemies->acquire("wizard", Bullet::Blue, glm::vec2(40, -20.0f));
	enemies->acquire("ninja", Bullet::Red, glm::vec2(-40, -20.0f));
}

void Model::level9() {
	(*(player)).set_position(glm::vec2(0,0));
	enemies->acquire("boss", Bullet::Red, glm::vec2(40, 50.0f));
}


