#include "Sprite.hpp"
#include <stdexcept>
#include <variant>

Sprite::Sprite(size_t z_) : z(z_) {}
Sprite::~Sprite() {};

void Sprite::set_position(const glm::vec2 &pos_) { pos = pos_; }
void Sprite::set_rotation(float rot_) { rot = rot_; }

void Sprite::queue_animation(Animation::Animation anim, bool loop, std::function<void ()> *on_finish) {
	animations.emplace_back(anim, loop, on_finish);
}

void Sprite::override_animation(Animation::Animation anim, bool loop, std::function<void ()> *on_finish) {
	animations.clear();
	queue_animation(anim, loop, on_finish);
}

void Sprite::update(float elapsed_) {

	if (animations.size() == 0) {
		elapsed = 0.0f;
		return;
	}

	if (std::get<0>(animations.front()).index() == 0) { // static

		elapsed = 0.0f;
		return;

	} else { // dynamic

		elapsed += elapsed_;
		std::shared_ptr<const Animation::Dynamic> d = std::get<1>(std::get<0>(animations.front())).lock();

		// move frames forward
		while (current_frame < d->size() && elapsed >= (*d)[current_frame].first) {
			elapsed -= (*d)[current_frame].first;
			current_frame++;
		}

		// either loop or go to next animation
		if (current_frame == d->size()) {

			current_frame = 0;
			if (!std::get<1>(animations.front())) { // don't loop

				auto func = std::get<2>(animations.front());
				if (func != nullptr) {
					(*func)();
				};

				animations.pop_front();

			}
			update(0.0f);

		}
	}
}

std::weak_ptr<const Animation::Static> Sprite::frame() {

	if (animations.size() == 0) {
		throw std::runtime_error("Sprite was asked to draw, but it had no animation");
	}

	auto anim = std::get<0>(animations.front());
	if (anim.index() == 0) { // static
		return std::get<0>(anim);
	} else {
		// i know, it's a bit ugly :(
		return (*std::get<1>(anim).lock())[current_frame].second;
	}

}
