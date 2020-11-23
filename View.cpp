#include "View.hpp"
#include "GL.hpp"
#include "data_path.hpp"
#include <cstdio>
#include <memory>
#include "yaml-cpp/yaml.h"

View::View() {

	glGenVertexArrays(1, &empty_vao);

	background_framebuffer = std::make_unique<BackgroundFramebuffer>(empty_vao);

	program = Framebuffer::compile_shader_from_path(
		data_path("shaders/fullscreen_quad_v.glsl"),
		data_path("shaders/small_to_big_f.glsl")
	);

	// get uniform locations
	program_ViewportOffset_uvec2 = glGetUniformLocation(program, "ViewportOffset");
	program_ViewportSize_uvec2 = glGetUniformLocation(program, "ViewportSize");
	program_TextureSize_uvec2 = glGetUniformLocation(program, "TextureSize");


}
View::~View() {}

void View::draw(const glm::uvec2 &drawable_size) {


	background_framebuffer->realloc(drawable_size);

	glViewport(0, 0, ScreenWidth, ScreenHeight);

	glUseProgram(background_framebuffer->program);
	glUniform1f(background_framebuffer->Time_float, total_time);
	glUseProgram(0);

	GLuint tex = background_framebuffer->draw(0);

	size_t scale = std::min(
		drawable_size.x * ScreenHeight,
		drawable_size.y * ScreenWidth
	);

	glm::uvec2 viewport_size {
		scale / ScreenHeight,
		scale / ScreenWidth
	};
	glm::uvec2 viewport_offset = (drawable_size - viewport_size);
	viewport_offset /= 2;

	glViewport(
		viewport_offset.x,
		viewport_offset.y,
		viewport_size.x,
		viewport_size.y
	);

	// draw to screen
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);

	glUniform2ui(program_ViewportOffset_uvec2, viewport_offset.x, viewport_offset.y);
	glUniform2ui(program_ViewportSize_uvec2, viewport_size.x, viewport_size.y);
	glUniform2ui(program_TextureSize_uvec2, ScreenWidth, ScreenHeight);

	glBindVertexArray(empty_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
	GL_ERRORS();

}

void View::update(float elapsed) {
	total_time += elapsed;
}
