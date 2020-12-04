#pragma once

#include "Animation.hpp"
#include "Bullet.hpp"
#include "Sprite.hpp"
#include "SpriteManager.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Ui {

	public:

		static constexpr float MARGIN_X = 8.0f;
		static constexpr float MARGIN_TOP = 8.0f;

		Ui();
		~Ui() = default;

		void set_cursor(const glm::vec2 &pos);
		void set_score(size_t score);
		void set_health(size_t health_);
		void set_health_color(Bullet::Color color);

		void update(float elapsed);
		void draw_field(std::vector<SpriteManager::Vertex> &verts);
		void draw_border(std::vector<SpriteManager::Vertex> &verts);

	private:
		SpriteManager on_field;
		SpriteManager on_border;

		Animation::Animation blue_heart;
		Animation::Animation blue_half_heart;

		Animation::Animation red_heart;
		Animation::Animation red_half_heart;

		std::vector<Animation::Animation> numbers;


		std::vector<std::shared_ptr<Sprite>> heart_sprites;
		std::shared_ptr<Sprite> cursor_sprite;
		std::vector<std::shared_ptr<Sprite>> label_sprites;
		std::vector<std::shared_ptr<Sprite>> score_sprites;

		glm::vec2 cursor {-1000.0f, -1000.0f};
		float ideal_score = 0.0f;
		float score = 0.0f;
		Bullet::Color health_color = Bullet::Red;
		size_t health = 0;


};
