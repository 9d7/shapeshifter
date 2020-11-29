#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <tuple>
#include "Animation.hpp"

class Sprite {

	public:

		~Sprite();

		void set_position(const glm::vec2 &pos_);
		void set_rotation(float rot_);

	private:
		static void do_nothing() {}
	public:

		void queue_animation(Animation::Animation anim, bool loop, std::function<void()> on_finish = do_nothing);
		void override_animation(Animation::Animation anim, bool loop, std::function<void()> on_finish = do_nothing);

		friend class SpriteManager;

	private:

		Sprite(size_t z_);

		const size_t z;

		glm::vec2    pos           {0, 0};
		float        rot           = 0;

		size_t       current_frame = 0;
		float        elapsed       = 0;
		std::list<std::tuple<Animation::Animation, bool, std::function<void()>>> animations;

		void update(float elapsed);
		std::weak_ptr<const Animation::Static> frame();
};
