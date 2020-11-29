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
			glm::vec2  size;
			glm::vec2  texture;
		};
		static_assert(sizeof(Vertex) == 4*2 + 4 + 4*2 + 4*2, "Vertex wrong size");

		void draw(std::vector<Vertex> &verts);

	protected:

		std::multimap<size_t, std::weak_ptr<Sprite>> sprites;

};


