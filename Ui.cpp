#include "Ui.hpp"
#include "Animation.hpp"

Ui::Ui() {

	blue_heart      = Animation::find_static("heart_blue");
	blue_half_heart = Animation::find_static("heart_half_blue");
	red_heart       = Animation::find_static("heart_red");
	red_half_heart  = Animation::find_static("heart_half_red");

	cursor_sprite = on_field.from_anim(Animation::find_static("cursor"), false);

	for (char l = '0'; l <= '9'; l++) {
		numbers.push_back(Animation::find_static(std::string(1, l)));
	}


	label_sprites.push_back(on_border.from_anim(Animation::find_static("score_label"), false));
	label_sprites.push_back(on_border.from_anim(Animation::find_static("lives_label"), false));

}

void Ui::set_cursor(const glm::vec2 &pos) {
	cursor = pos;
	cursor_sprite->set_position(pos);
}

void Ui::set_score(size_t score) {
	ideal_score = (float)score;
}

void Ui::set_health(size_t health_) {
	health = health_;
}

void Ui::set_health_color(Bullet::Color color) {
	health_color = color;
}

void Ui::draw_border(std::vector<SpriteManager::Vertex> &verts) {
	on_border.draw(verts);
}

void Ui::draw_field(std::vector<SpriteManager::Vertex> &verts) {
	on_field.draw(verts);
}
