#pragma once

#include "Sprite.hpp"
#include <glm/glm.hpp>
#include <memory>

class Bullet {
	public:
		friend class BulletManager;

		enum Color {
			Blue,
			Red,
			Purple
		};

		Bullet(
			const glm::vec2         &position_,
			const glm::vec2         &velocity_,
			Color                   color_,
			std::shared_ptr<Sprite> sprite_,
			bool                    shot_by_player_
		) : position(position_), velocity(velocity_), color(color_), sprite(sprite_), shot_by_player(shot_by_player_) {};

		glm::vec2 get_position() const;
		float     get_age() const;
		glm::vec2 get_velocity() const;
		void      update(float elapsed);
		bool      from_player() const;
		Color     get_color() const;

	private:

		glm::vec2               position;
		glm::vec2               velocity;
		Color                   color;
		std::shared_ptr<Sprite> sprite;
		bool                    shot_by_player;

		float                   age = 0.0f;
};
