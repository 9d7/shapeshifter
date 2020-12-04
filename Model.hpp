#pragma once

#include <memory>
#include "Animation.hpp"
#include "BulletManager.hpp"
#include "Enemy.hpp"
#include "EnemyManager.hpp"
#include "Sprite.hpp"
#include "View.hpp"

class Model {

	public:

		static constexpr float MOVE_FORCE   = 1200.0f;  // Maybe as TODO below?
		static constexpr float MAX_VELOCITY = 300.0f;   // TODO make a multiplier that we can adjust on the fly if we want powerups or slowing abilities
		static constexpr float FRICTION     = 0.95f;    // Currently unused
		static constexpr float FRICTION_FORCE = 500.0f; // Currently used for force based friction
		static constexpr float BULLET_SPEED = 300.0f;   // Same TODO as above
		static constexpr float CAMERA_SMOOTHNESS = 0.075f; // [0, 1], lower = smoother

		Model(std::shared_ptr<View> view_);

		void update(float elapsed);

		// it might be a good idea to move player stuff into its own
		// class? not really sure
		void player_move(const glm::vec2 &direction);
		void player_shoot(Bullet::Color color);
		void player_color(Bullet::Color color);

		void set_mouse_position(const glm::vec2 &position);

	protected:

		std::shared_ptr<View> view;
		std::shared_ptr<BulletManager> bullets;
		std::shared_ptr<EnemyManager> enemies;

		void update_view();

		// player
		Bullet::Color player_col = Bullet::Color::Blue;
		Animation::Animation player_red;
		Animation::Animation player_blue;
		std::shared_ptr<Sprite> player_sprite;
		glm::vec2 player_force    {0.0f, 0.0f};
		glm::vec2 player_velocity {0.0f, 0.0f};
		glm::vec2 player_position {0.0f, 0.0f};
		glm::vec2 player_friction {0.0f, 0.0f};

		// camera
		glm::vec2 ideal_camera_position {0.0f, 0.0f};
		glm::vec2 camera_position {0.0f, 0.0f};

		// mouse
		glm::vec2 mouse_ui_position {0.0f, 0.0f};
		glm::vec2 mouse_world_position {0.0f, 0.0f};
};
