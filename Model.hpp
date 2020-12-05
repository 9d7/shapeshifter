#pragma once

#include <memory>
#include "Animation.hpp"
#include "BulletManager.hpp"
#include "Enemy.hpp"
#include "EnemyManager.hpp"
#include "Sprite.hpp"
#include "View.hpp"
#include "Player.hpp"

class Model {

	public:
		static constexpr float CAMERA_SMOOTHNESS = 0.075f; // [0, 1], lower = smoother
		static constexpr float BULLET_SPEED = Player::BULLET_SPEED; // Current hack to get this working for BulletShooter.cpp

		Model(std::shared_ptr<View> view_);

		void update(float elapsed);

		// Wrappers for player functions so that only Model directly interacts with player
		void player_shoot(); //wrapper for Player::shoot
		void player_move(glm::vec2 direction);
		void set_player_color(Bullet::Color color);

		void set_mouse_position(const glm::vec2 &position);

		float get_bullet_speed() const; // TODO change this so different enemy types have the ability to set their own bullet speeds

	protected:

		std::shared_ptr<View> view;
		std::shared_ptr<BulletManager> bullets;
		std::shared_ptr<EnemyManager> enemies;
		std::shared_ptr<Player> player;

		void update_view(float elapsed);

		// camera
		glm::vec2 ideal_camera_position {0.0f, 0.0f};
		glm::vec2 camera_position {0.0f, 0.0f};

		// mouse
		glm::vec2 mouse_ui_position {0.0f, 0.0f};
		glm::vec2 mouse_world_position {0.0f, 0.0f};
};
