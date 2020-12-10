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

Load< Sound::Sample > enemy_hit(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("enemy_hit.wav"));
});

Load< Sound::Sample > player_shoot_sound(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("shoot.wav"));
});

Load< Sound::Sample > background(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("ericadventure3-bad.wav"));
	});

Model::Model(std::shared_ptr<View> view_) : view(view_) {
	//Sound::loop(*background);
	player = std::make_shared<Player>(view);

	bullets = std::make_shared<BulletManager>();

	Model::enemies = std::make_shared<EnemyManager>(view->sprites, bullets);

	level = std::make_shared<Level>(enemies);
	start();
	//enemies->acquire("soldier", Bullet::Blue, glm::vec2(0, 5.0f));
	//enemies->acquire("soldier", Bullet::Red, glm::vec2(0, -5.0f));

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
		Mode::set_current(std::make_shared< MenuMode >(MenuMode::Message::Died));
		reset_player();
		view->ui->set_health(player->get_lives()); // TODO maybe include this in reset_player so you refresh lives at a new level?
		view->ui->set_score(0);
	}

	player->update(elapsed);
	enemies->update(elapsed, player->get_position());
	static size_t score = 0; //TODO this is dumb, make it a member variable

	bullets->update(elapsed);
	Level::LevelStatus level_status = level->update(elapsed);
	if (level_status == Level::NextLevel) { // if starting a new level, reset the player and teleport where you need them to be
		reset_player(level->get_spawn_point(level->get_current_level()));
		bind_player();
	}
	else if (level_status == Level::NextRoom) { // TODO pause this until player is close enough to the new center
		set_player_position(level->get_room_center(level->get_current_room())); // TODO remove when above is done and replace with unbind until new center is reached
		bind_player();
	}


	for (BulletManager::iterator b_it = bullets->begin(); b_it != bullets->end();) {

		bool should_erase = false;
		const Bullet b = **b_it;
		if ((*b_it)->get_age() > 3.0f) {
			should_erase = true;
		} else {

			if (b.from_player()) {
				for (EnemyManager::iterator e_it = enemies->begin(); e_it != enemies->end();) {
					const Enemy e = **e_it;
					float radius = (e.size().x + e.size().y) / 4.0f; // average of w and h, over 2
					if ((glm::length(e.position() - b.get_position()) < radius + 4.0f && e.get_color() != b.get_color()) ||
						((**(e_it)).move_style == Enemy::MovementStyle::Boss && glm::length(e.position() - b.get_position()) < 8.0f + 4.0f)) {
						// kill enemy and bullet
						int health = (**(e_it)).take_damage(1);
						if (health == 0) {
							Sound::play(*enemy_die, 1.0f, 0.0f);
							if ((**(e_it)).move_style == Enemy::MovementStyle::Turret) (**(e_it)).dead_turret();
							else e_it = enemies->erase(e_it);
							
						} else Sound::play(*enemy_hit, 1.0f, 0.0f);
						score += 100;
						view->ui->set_score(score);

						should_erase = true;
						break;
					} else {
						e_it++;
					}
				}
			} else {
				if (b.get_color() != player->get_color() && glm::length(player->get_position() - b.get_position()) < 8.0f + 4.0f) {
					if (player->get_lives() > 0) //player->hit(); TODOTODOTODO READD++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

	hunter_kill();

	// update camera to be out of dead space
	static const glm::vec2 MARGIN = glm::vec2(
			View::FieldWidth,
			View::FieldHeight
	) * 1.0f / 4.0f;

	//glm::vec2 space_from_camera_center = ideal_camera_position - player->get_position();
	//glm::vec2 mouse_camera_from_player = (mouse_world_position) - player->get_position();
	//float asymptote = glm::length(mouse_camera_from_player) / (glm::length(mouse_camera_from_player) + 10.0f) ;

	ideal_camera_position = 0.4f * mouse_world_position + 0.6f * player->get_position();
	
	
	camera_position += (ideal_camera_position - camera_position) * glm::pow(CAMERA_SMOOTHNESS, 60 * elapsed);
	
	//printf("player %f %f ||| camera %f %f ||| mouse %f %f\n", player->get_position().x, player->get_position().y, view->get_camera_position().x, view->get_camera_position().y, mouse_world_position.x, mouse_world_position.y);
	//printf("ideal %f %f ||| asymptote %f ||| 1.0 - %f\n", ideal_camera_position.x, ideal_camera_position.y, asymptote, 1.0f - asymptote);
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

void Model::bind_player() {
	player->bind(level->get_room_center(level->get_current_room()), level->get_room_bounds(level->get_current_room()));
}

void Model::set_player_position(glm::vec2 new_position) {
	player->set_position(new_position);
	view->update_camera(new_position);
}

void Model::reset_player(glm::vec2 reset_position) {
	player->reset_player(reset_position);
	view->update_camera(reset_position);
}

float Model::get_bullet_speed() const {
	return Player::BULLET_SPEED;
}

void Model::hunter_kill() {
	for (EnemyManager::iterator e_it = enemies->begin(); e_it != enemies->end();) {
		if ((**e_it).move_style == Enemy::MovementStyle::Hunter &&
			(**e_it).get_color() != player->get_color()
			&& glm::length(player->get_position() - (**e_it).position()) < 8.0f + 4.0f) {	
			player->hit();
			view->ui->set_health(player->get_lives());
			e_it = enemies->erase(e_it);
			Sound::play(*enemy_die, 1.0f, 0.0f);
		} else e_it++;
	}
}

void Model::kill_turret(Enemy *e) {
	for (EnemyManager::iterator e_it = enemies->begin(); e_it != enemies->end(); e_it++) {
		if (&(**e_it) == e) {
			e_it = enemies->erase(e_it);
			return;
		}
	}
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

void Model::start() {
	level->start_level(0);
	reset_player(level->get_spawn_point(0));
	bind_player();
}

bool Model::turrets_dead() {
	int count = 0;
	for (EnemyManager::iterator e_it = enemies->begin(); e_it != enemies->end(); e_it++) {
		if ((*e_it)->move_style == Enemy::MovementStyle::Deadturret) {
			count++;
		}
	}

	return count == enemies->enemies.size();
}

void Model::spawn_hunter(glm::vec2 pos, Bullet::Color col) {
	enemies->acquire("hunter", col, pos);
}
