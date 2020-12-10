#pragma once

#include <memory>
#include "Animation.hpp"
#include "BulletManager.hpp"
#include "View.hpp"
#include <variant>
class Player {

	public:
		
		Player(std::shared_ptr<View> view);
		~Player() = default;

		static constexpr float MOVE_FORCE = 1000.0f;  // Maybe as TODO below?
		static constexpr float MAX_VELOCITY = 150.0f;   // TODO make a multiplier that we can adjust on the fly if we want powerups or slowing abilities
		static constexpr float FRICTION_FORCE = 600.0f; // Currently used for force based friction
		static constexpr float BULLET_SPEED = 300.0f;   // Same TODO as above

		enum AssistMode {
			Off,
			Mouse,
			Closest,
			Furthest,
			Homing
		};

		// Update functions
		void update(float elapsed);
		void update_sprite();

		// Player functions
		void      move(const glm::vec2& direction);
		glm::vec2 shoot(const glm::vec2 target_position); // TODO create another shoot function or add a parameter to change whether direction is based on mouse pos or player direction
		void      reset_player(
			glm::vec2 reset_position = glm::vec2(0.0f), 
			glm::vec2 reset_velocity = glm::vec2(0.0f), 
			float reset_rotation = 0.0f
		);
		void      hit();
		void      bind(glm::vec2 center, glm::vec2 bounds);
		void      unbind();

		// set/get functions
		void set_lives(int new_lives);
		void set_color(Bullet::Color new_color);
		void set_rotation(float new_rotation); // Hard set an angle, disregarding speed limits. TODO add a parameter or refactor this when multilayer sprites are in
		void set_rotation(float new_rotation, float elapsed); // Sets an angle within rotation speed limits
		void set_position(glm::vec2 new_position);
		void set_velocity(glm::vec2 new_velocity);
		void set_sprite(std::shared_ptr<Sprite> new_sprite);
		void set_aim_assist_angle(float new_assist_angle);
		void set_aim_assist_mode(AssistMode new_assist_mode);
		void set_rotation_limit(float new_rotation_limit);
		void set_rotation_lock(bool new_rotation_lock);

		glm::vec2               get_position() const;
		glm::vec2               get_velocity() const;
		std::shared_ptr<Sprite> get_sprite() const; // Don't use unless you know what you're doing, not sure if this works as intended
		Bullet::Color           get_color() const;
		float                   get_rotation() const;
		AssistMode              get_assist_mode() const;
		int                     get_lives() const;
		float                   get_cd() const;

		bool is_bound();
		

	protected:
		// Values needing to be set
		Animation::Animation player_red;
		Animation::Animation player_blue;
		std::shared_ptr<Sprite> sprite;

		// Editable values with defaults
		float shot_cd = 0.0f;
		bool bound = false;
		int lives = 20;
		float hit_cd = 1.0f;
		Bullet::Color color = Bullet::Color::Blue;
		bool rotation_lock_active = false;
		float rotation = 0.0f;                     // Current rotation
		float rotation_limit = 9.0f;               // Max number of rads/s, 0 is locked rotation, <0 is no limit
		float aim_assist_angle = 0.3f;             // [0,1], representing angle of cone where shot goes towards enemy. 0 is no aim assist, 1 is hax.
		AssistMode assist_mode = Off;              // Current aim assist mode

		// Movement vectors
		glm::vec2 force    { 0.0f, 0.0f };
		glm::vec2 velocity { 0.0f, 0.0f };
		glm::vec2 position { 0.0f, 0.0f };
		glm::vec2 friction { 0.0f, 0.0f };

		// Bounding coords
		float max_x = 0.0f;
		float max_y = 0.0f;
		float min_x = 0.0f;
		float min_y = 0.0f;
};