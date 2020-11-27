#include "SpriteManager.hpp"
#include <memory>

#include <cstdio>

SpriteManager::SpriteManager() {}
SpriteManager::~SpriteManager() {}

void SpriteManager::fix_zombies() {

	if (sprites.size() == 0) return;

	static constexpr float MAX_RATIO = 0.5f;
	if (zombie_count * 1.0f / sprites.size() > MAX_RATIO) {

		zombie_count = 0;
		for (auto it = sprites.begin(); it != sprites.end();) {
			if (it->second.expired()) {
				printf("Zombie reaped\n");
				it = sprites.erase(it);
			} else {
				it++;
			}
		}
	}

}

std::shared_ptr<Sprite> SpriteManager::acquire(size_t z) {

	std::shared_ptr<Sprite> sprite(
		new Sprite(z),
		[this](Sprite *s){ zombie_count++; printf("Sprite killed\n"); delete s; fix_zombies(); }
	);

	sprites.emplace(z, sprite);
	printf("Sprite spawned\n");

	fix_zombies();
	return sprite;

}

void SpriteManager::draw(std::vector<Vertex> &verts) {

	verts.clear();
	for (const auto &s_it : sprites) {
		std::shared_ptr<Sprite> spr = s_it.second.lock();
		std::shared_ptr<const Animation::Static> frame = spr->frame().lock();

		for (uint32_t i = 0; i < 6; i++) {

			verts.emplace_back(Vertex{
				spr->pos,
				spr->rot,
				glm::uvec2(frame->x, frame->y),
				glm::uvec2(frame->w, frame->h),
				i
			});
		}

	}

}
