#pragma once

#include <memory>
#include "Level.hpp"
#include "Animation.hpp"
#include "Bullet.hpp"
#include "BulletManager.hpp"
#include "Enemy.hpp"
#include "EnemyManager.hpp"
#include "Sprite.hpp"
#include "View.hpp"
#include "Player.hpp"
#include "Sound.hpp"

class Model {

	public:
		static constexpr float CAMERA_SMOOTHNESS = 0.050f; // [0, 1], lower = smoother
		static constexpr float BULLET_SPEED = Player::BULLET_SPEED; // Current hack to get this working for BulletShooter.cpp

		Model(std::shared_ptr<View> view_);

		void update(float elapsed);

		// Wrappers for player functions so that only Model directly interacts with player
		void player_shoot(); //wrapper for Player::shoot
		void player_move(glm::vec2 direction);
		void set_player_color(Bullet::Color color);
		void lock_rotation(bool rotation_lock);
		void bind_player();
		void set_player_position(glm::vec2 new_position);
		void reset_player(glm::vec2 reset_position = {0.0f, 0.0f});

		void set_mouse_position(const glm::vec2 &position);

		

		float get_bullet_speed() const; // TODO change this so different enemy types have the ability to set their own bullet speeds

		void hunter_kill();
		bool turrets_dead();
		static void kill_turret(Enemy *e);

		static std::shared_ptr<Enemy> get_closest(glm::vec2 enemy_pos);
		static void spawn_hunter(glm::vec2 pos, Bullet::Color col);

		void start();

	protected:

		std::shared_ptr<View> view;
		std::shared_ptr<BulletManager> bullets;
		static std::shared_ptr<EnemyManager> enemies;
		std::shared_ptr<Player> player;
		std::shared_ptr<Level> level;

		void update_view(float elapsed);

		// camera
		glm::vec2 ideal_camera_position {0.0f, 0.0f};
		glm::vec2 camera_position {0.0f, 0.0f};

		// mouse
		glm::vec2 mouse_ui_position {0.0f, 0.0f};
		glm::vec2 mouse_world_position {0.0f, 0.0f};
		const glm::vec2 ui_center_position{ 160.0f, 120.0f };
};
