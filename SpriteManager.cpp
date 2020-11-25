#include "SpriteManager.hpp"
#include <memory>

void SpriteManager::fix_zombies() {

	if (sprites.size() == 0) return;

	static constexpr float MAX_RATIO = 0.5f;
	if (zombie_count * 1.0f / sprites.size() > MAX_RATIO) {

		zombie_count = 0;
		for (auto it = sprites.begin(); it != sprites.end();) {
			if (it->second.expired()) {
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
		[this](Sprite *s){ zombie_count++; delete s; fix_zombies(); }
	);

	sprites.emplace(z, std::weak_ptr(sprite));

	fix_zombies();
	return sprite;

}
