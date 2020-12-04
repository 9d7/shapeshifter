#include "Stars.hpp"
#include "glm/common.hpp"
#include <chrono>

std::mt19937 Stars::mt ((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
std::uniform_real_distribution<float> Stars::dist (0.0f, 1.0f);

Stars::Stars(uint32_t num_stars) {

	stars.reserve(num_stars);
	star_tex_coords.reserve(num_stars);

	star_animations = {
		std::get<1>(Animation::find_dynamic("star_pip")),
		std::get<1>(Animation::find_dynamic("star_twinkle")),
		std::get<1>(Animation::find_dynamic("star_expand")),
		std::get<1>(Animation::find_dynamic("star_burst")),
		std::get<1>(Animation::find_dynamic("star_shoot")),
		std::get<1>(Animation::find_dynamic("star_x"))
	};

	for (size_t i = 0; i < num_stars; i++) {

		Star s;
		s.animation_index = (size_t)glm::floor(dist(mt) * star_animations.size());
		s.frame_index = 0;
		s.elapsed = dist(mt) * 100.0f; // randomize phase
		s.speed_modifier = 0.95f + dist(mt) * 0.10f;
		stars.push_back(s);

		const Animation::Static tc = *(*star_animations[s.animation_index].lock())[0].second.lock();
		star_tex_coords.emplace_back(tc.x, tc.y);


	}

	update(0.0f);
}

void Stars::update(float elapsed) {

	for (size_t i = 0; i < stars.size(); i++) {

		Star &s = stars[i];

		s.elapsed += elapsed;
		std::shared_ptr<const Animation::Dynamic> d = star_animations[s.animation_index].lock();

		// move frames forward
		bool changed_frames = false;
		while (s.frame_index < d->size() && s.elapsed >= (*d)[s.frame_index].first) {
			s.elapsed -= (*d)[s.frame_index].first;
			s.frame_index++;
			s.frame_index %= d->size();
			changed_frames = true;
		}

		if (changed_frames) {
			const Animation::Static tc = *(*d)[s.frame_index].second.lock();
			star_tex_coords[i] = glm::vec2(tc.x, tc.y);
		}

	}

}
