#include "SpriteManager.hpp"
#include <memory>

#include <cstdio>

SpriteManager::SpriteManager() {}
SpriteManager::~SpriteManager() {}

std::shared_ptr<Sprite> SpriteManager::acquire(size_t z) {

	std::shared_ptr<Sprite> sprite(
		new Sprite(z)
	);

	sprites.emplace(z, sprite);
	printf("Sprite spawned\n");

	return sprite;

}

void SpriteManager::draw(std::vector<Vertex> &verts) {

	verts.clear();
	for (auto s_it = sprites.cbegin(); s_it != sprites.cend();) {
		if (s_it->second.expired()) {
			s_it = sprites.erase(s_it);
			continue;
		}

		std::shared_ptr<Sprite> spr = s_it->second.lock();
		std::shared_ptr<const Animation::Static> frame = spr->frame().lock();

		for (uint32_t i = 0; i < 6; i++) {

			verts.emplace_back(Vertex{
				spr->pos,
				spr->rot,
				glm::vec2(frame->w, frame->h),
				glm::vec2(frame->x, frame->y)
			});
		}

		s_it++;
	}

}
