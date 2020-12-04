#pragma once

#include "Framebuffer.hpp"
class BackgroundFramebuffer : public Framebuffer {

	public:

		BackgroundFramebuffer(GLuint empty_vao);
		GLuint draw(GLuint old_tex) override;

		void realloc(const glm::uvec2 &drawable_size) override;

		GLint Time_float = 0;
		GLint Camera_vec2 = 0;
		GLint Parallax_float = 0;

	private:
		GLuint blue_noise_tex = 0;
};
