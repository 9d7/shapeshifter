#include "Stars.hpp"
#include "Renderer.hpp"
#include "gl_errors.hpp"
#include "glm/fwd.hpp"

Stars::Stars() {
	for (Star &s : stars) {

		s.animation_index = (size_t)glm::floor(real(mt) * animations.size());
		s.elapsed = real(mt) * 30.0f; // phase offset
		s.frame_index = 0;
		s.speed_modifier = 0.95f + real(mt) * 0.1f;

		s.parallax = real(mt) * 0.75f + 0.25f;
		s.color = glm::vec4(1.0f, real(mt) * 0.25f + 0.75f, real(mt) * 0.75f, s.parallax - 0.15f);
		s.pos = glm::vec2(
			real(mt) * (float)Renderer::ScreenWidth * 2.0f,
			real(mt) * (float)Renderer::ScreenHeight * 2.0f
		);
	}
}

Stars::~Stars() {}

void Stars::update(float elapsed) {

	for (Star &s : stars) {
		s.elapsed += elapsed * s.speed_modifier;

		while (s.elapsed >= animations[s.animation_index][s.frame_index].second) {
			s.elapsed -= animations[s.animation_index][s.frame_index].second;
			s.frame_index++;
			s.frame_index %= animations[s.animation_index].size();
		}

		s.tex_coords = animations[s.animation_index][s.frame_index].first;
	}


}

void Stars::draw(
		const std::array<GLint, NUM_INSTANCES> &TexCoords_ivec2, 
		const std::array<GLint, NUM_INSTANCES> &Colors_vec4, 
		const std::array<GLint, NUM_INSTANCES> &Positions_ivec2, 
		const std::array<GLint, NUM_INSTANCES> &Parallax_float
		) {
	
	for (size_t i = 0; i < NUM_INSTANCES; i++) {
		
		const Star &s = stars[i];
		glUniform2i(TexCoords_ivec2[i], s.tex_coords.x, s.tex_coords.y);
		glUniform4f(Colors_vec4[i], s.color.r, s.color.g, s.color.b, s.color.a);
		glUniform2i(Positions_ivec2[i], s.pos.x, s.pos.y);
		glUniform1f(Parallax_float[i], s.parallax);

	}
}
