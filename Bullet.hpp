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
			std::shared_ptr<Sprite> sprite_
		) : position(position_), velocity(velocity_), color(color_), sprite(sprite_) {};

		glm::vec2 get_position();
		float     get_age();
		void      update(float elapsed);

	private:

		glm::vec2               position;
		glm::vec2               velocity;
		Color                   color;
		std::shared_ptr<Sprite> sprite;

		float                   age = 0.0f;
};
