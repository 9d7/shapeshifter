#include "Ui.hpp"
#include "Animation.hpp"
#include "View.hpp"
#include "glm/exponential.hpp"

Ui::Ui() {

	blue_heart      = Animation::find_static("heart_blue");
	blue_half_heart = Animation::find_static("heart_half_blue");
	red_heart       = Animation::find_static("heart_red");
	red_half_heart  = Animation::find_static("heart_half_red");

	cursor_sprite = on_field.from_anim(Animation::find_static("cursor"), false);

	for (char l = '0'; l <= '9'; l++) {
		numbers.push_back(Animation::find_static(std::string(1, l)));
	}


	label_sprites.push_back(on_border.from_anim(Animation::find_static("lives_label"), false));
	label_sprites.push_back(on_border.from_anim(Animation::find_static("score_label"), false));

	const Animation::Static lives_anim = *(label_sprites[0]->frame().lock());
	const Animation::Static score_anim = *(label_sprites[1]->frame().lock());

	lives_label_size = glm::vec2((float)lives_anim.w, (float)lives_anim.h);
	score_label_size = glm::vec2((float)score_anim.w, (float)score_anim.h);

	label_sprites[0]->set_position(glm::vec2(
		lives_label_size.x / 2.0f + MARGIN_X,
		View::ScreenHeight - MARGIN_TOP
	));

	label_sprites[1]->set_position(glm::vec2(
		View::ScreenWidth - score_label_size.x / 2.0f - MARGIN_X - 8.0f * (NUM_DIGITS + 1),
		View::ScreenHeight - MARGIN_TOP
	));

	for (size_t i = 0; i < NUM_DIGITS; i++) {
		score_sprites.push_back(on_border.from_anim(numbers[0], false));
		score_sprites.back()->set_position(glm::vec2(
			View::ScreenWidth - 4.0f - MARGIN_X - 8.0f * i,
			View::ScreenHeight - MARGIN_TOP
		));
	}

}

void Ui::update(float elapsed) {

	score += (ideal_score - score) * glm::pow(SCORE_SPEED, 60 * elapsed);

	size_t new_score = (size_t)glm::round(score);
	if (new_score != score_to_render) {
		score_to_render = new_score;
		update_score();
	}

}

void Ui::set_cursor(const glm::vec2 &pos) {
	cursor = pos;
	cursor_sprite->set_position(pos);
}

void Ui::set_score(size_t score) {
	ideal_score = (float)score;
}

void Ui::set_health(size_t health_) {
	if (health_ == health) return;

	health = health_;
	update_hearts();
}

void Ui::set_health_color(Bullet::Color color) {
	if (color == health_color) return;

	health_color = color;
	update_hearts();
}

void Ui::update_hearts() {

	size_t num_hearts = (health + 1) / 2;

	while (heart_sprites.size() < num_hearts) {
		heart_sprites.push_back(on_border.acquire(0));
		heart_sprites.back()->set_position(glm::vec2(
			lives_label_size.x + MARGIN_X + 4.0 + 8.0 * heart_sprites.size(),
			View::ScreenHeight - MARGIN_TOP
		));
	}

	while (heart_sprites.size() > num_hearts) {
		heart_sprites.pop_back();
	}

	Animation::Animation heart_anim = health_color == Bullet::Blue ? blue_heart : red_heart;
	for (std::shared_ptr<Sprite> s : heart_sprites) {
		s->override_animation(heart_anim, false);
	}

	if (health % 2 == 1) {
		Animation::Animation half_anim = (health_color == Bullet::Blue) ? blue_half_heart : red_half_heart;
		heart_sprites.back()->override_animation(half_anim, false);
	}
}

void Ui::update_score() {

	size_t new_score = score_to_render;

	for (size_t i = 0; i < NUM_DIGITS; i++) {
		score_sprites[i]->override_animation(numbers[new_score % 10], false);
		new_score /= 10;
	}
}

void Ui::draw_border(std::vector<SpriteManager::Vertex> &verts) {
	on_border.draw(verts);
}

void Ui::draw_field(std::vector<SpriteManager::Vertex> &verts) {
	on_field.draw(verts);
}
