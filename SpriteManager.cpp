#include "SpriteManager.hpp"
#include "Sound.hpp"
#include <memory>

#include <cstdio>

SpriteManager::SpriteManager() {}
SpriteManager::~SpriteManager() {}

std::shared_ptr<Sprite> SpriteManager::acquire(int z) {

	std::shared_ptr<Sprite> sprite(
		new Sprite(z)
	);

	sprites.emplace(z, sprite);

	return sprite;

}

std::shared_ptr<Sprite> SpriteManager::from_anim(Animation::Animation anim, bool loop) {

	if (anim.index() == 0) { // static
		// if static, get z value straight from struct
		std::shared_ptr<Sprite> s = acquire(std::get<0>(anim).lock()->z);
		s->override_animation(anim, false);
		return s;

	} else {

		std::shared_ptr<Sprite> s = acquire(std::get<1>(anim).lock()->front().second.lock()->z);
		s->override_animation(anim, loop);
		return s;

	}

}

void SpriteManager::update(float elapsed) {
	for (auto s = sprites.begin(); s != sprites.end();) {

		if (s->second.expired()) {
			s = sprites.erase(s);
			continue;
		}

		s->second.lock()->update(elapsed);

		s++;
	}
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
