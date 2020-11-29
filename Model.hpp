#pragma once

#include <memory>
#include "BulletManager.hpp"
#include "Sprite.hpp"
#include "View.hpp"

class Model {

	public:

		static constexpr float MOVE_FORCE   = 600.0f;
		static constexpr float MAX_VELOCITY = 400.0f;
		static constexpr float FRICTION     = 0.95f;
		static constexpr float BULLET_SPEED = 250.0f;

		Model(std::shared_ptr<View> view_);

		void update(float elapsed);

		void player_move(const glm::vec2 &direction);
		void player_shoot(Bullet::Color color);

		void set_mouse_position(const glm::vec2 &position);

	protected:

		std::shared_ptr<View> view;
		std::shared_ptr<BulletManager> bullets;

		void update_view();

		// player
		std::shared_ptr<Sprite> player_sprite;
		glm::vec2 player_force    {0.0f, 0.0f};
		glm::vec2 player_velocity {0.0f, 0.0f};
		glm::vec2 player_position {0.0f, 0.0f};

		// camera
		glm::vec2 camera_position {0.0f, 0.0f};

		// mouse
		glm::vec2 mouse_ui_position {0.0f, 0.0f};
		glm::vec2 mouse_world_position {0.0f, 0.0f};
};
