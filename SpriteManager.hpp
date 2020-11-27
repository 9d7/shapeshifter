#pragma once

#include <cstddef>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <unordered_map>
#include "GL.hpp"
#include "Sprite.hpp"

class SpriteManager {
	public:

		SpriteManager();
		~SpriteManager();

		// z here is the z-index: it cannot be changed.
		std::shared_ptr<Sprite> acquire(size_t z);

		struct Vertex {
			glm::vec2  position;
			float      rotation;
			glm::uvec2 size;
			glm::uvec2 texture;
			uint32_t   vertex;
		};
		static_assert(sizeof(Vertex) == 4*2 + 4 + 4*2 + 4*2 + 4, "Vertex wrong size");

		void draw(std::vector<Vertex> &verts);

	protected:
		// main idea: when a sprite dies, do nothing--only worry about
		// cleaning up the multimap once a significant portion of the
		// map is filled with "zombie sprites"
		size_t zombie_count;
		void fix_zombies();

		std::multimap<size_t, std::weak_ptr<Sprite>> sprites;

};


