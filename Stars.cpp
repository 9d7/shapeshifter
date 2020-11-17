#include "Stars.hpp"
#include "Renderer.hpp"
#include "gl_errors.hpp"
#include "glm/fwd.hpp"

Stars::Stars() {
	for (Star &s : stars) {
		s.color = glm::vec4(1.0f, 1.0f, 0.0f, 0.5f);
		s.tex_coords = glm::ivec2(0.0f, 16.0f);
		s.parallax = real(mt) * 0.75f + 0.25f;
		s.pos = glm::vec2(
			real(mt) * (float)Renderer::ScreenWidth * 2.0f,
			real(mt) * (float)Renderer::ScreenHeight * 2.0f
		);
	}
}

Stars::~Stars() {}

void Stars::update(float elapsed) {

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
