#pragma once

#include <memory>
#include "Animation.hpp"
#include "BulletManager.hpp"
#include "View.hpp"

class Player {

	public:

		Player(std::shared_ptr<View> view);
		~Player() = default;

		static constexpr float MOVE_FORCE = 1200.0f;  // Maybe as TODO below?
		static constexpr float MAX_VELOCITY = 250.0f;   // TODO make a multiplier that we can adjust on the fly if we want powerups or slowing abilities
		static constexpr float FRICTION = 0.95f;    // Currently unused
		static constexpr float FRICTION_FORCE = 500.0f; // Currently used for force based friction
		static constexpr float BULLET_SPEED = 300.0f;   // Same TODO as above

		// Update functions
		void update(float elapsed);
		void update_sprite();

		// Player functions
		void move(const glm::vec2& direction);
		glm::vec2 shoot(const glm::vec2 mouse_world_position); // TODO create another shoot function or add a parameter to change whether direction is based on mouse pos or player direction
		void reset_player();

		// set/get functions
		void set_color(Bullet::Color new_color);
		void set_rotation(float new_rotation); // TODO add a parameter or refactor this when multilayer sprites are in
		void set_position(glm::vec2 new_position);
		void set_velocity(glm::vec2 new_velocity);
		void set_sprite(std::shared_ptr<Sprite> new_sprite);

		glm::vec2 get_position() const;
		glm::vec2 get_velocity() const;
		std::shared_ptr<Sprite> get_sprite(); // Don't use unless you know what you're doing, not sure if this works as intended
		Bullet::Color get_color() const;
		

	protected:
		Bullet::Color color = Bullet::Color::Blue;
		Animation::Animation player_red;
		Animation::Animation player_blue;
		std::shared_ptr<Sprite> sprite;
		glm::vec2 force{ 0.0f, 0.0f };
		glm::vec2 velocity{ 0.0f, 0.0f };
		glm::vec2 position{ 0.0f, 0.0f };
		glm::vec2 friction{ 0.0f, 0.0f };
};