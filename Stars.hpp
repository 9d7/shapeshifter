#pragma once

#include <vector>
#include <memory>
#include "Animation.hpp"
#include <glm/glm.hpp>
#include <random>

class Stars {

	public:

		Stars(uint32_t num_stars);
		~Stars() = default;
		void update(float elapsed);

		std::vector<glm::vec2> star_tex_coords;

	protected:

		static std::mt19937 mt;
		static std::uniform_real_distribution<float> dist;


		struct Star {
			size_t     animation_index;
			size_t     frame_index;
			float      elapsed;
			// slightly modify speed of different patterns so they
			// don't overlap
			float      speed_modifier;
		};

		std::vector<Star> stars;
		std::vector<std::weak_ptr<const Animation::Dynamic>> star_animations;

};
