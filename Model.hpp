#pragma once

#include <memory>
#include "Sprite.hpp"
#include "View.hpp"

class Model {

	public:

		static constexpr float MOVE_FORCE   = 600.0f;
		static constexpr float MAX_VELOCITY = 400.0f;
		static constexpr float FRICTION     = 0.95f;

		Model(std::shared_ptr<View> view_);

		void update(float elapsed);

		void control_player(const glm::vec2 &direction);

	protected:

		void update_view();
		
		std::shared_ptr<Sprite> player_sprite;

		std::shared_ptr<View> view;

		glm::vec2 player_force    {0.0f, 0.0f};
		glm::vec2 player_velocity {0.0f, 0.0f};
		glm::vec2 player_position {0.0f, 0.0f};

		glm::vec2 camera_position {0.0f, 0.0f};

};
