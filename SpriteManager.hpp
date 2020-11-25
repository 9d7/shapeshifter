#pragma once

#include <cstddef>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <unordered_map>
#include "Sprite.hpp"

class SpriteManager {
	public:

		// z here is the z-index: it cannot be changed.
		std::shared_ptr<Sprite> acquire(size_t z);

	protected:
		// main idea: when a sprite dies, do nothing--only worry about
		// cleaning up the multimap once a significant portion of the
		// map is filled with "zombie sprites"
		size_t zombie_count;
		void fix_zombies();

		std::multimap<size_t, std::weak_ptr<Sprite>> sprites;
};


