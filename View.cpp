#include "View.hpp"
#include "GL.hpp"
#include "data_path.hpp"
#include <cstdio>
#include <memory>

View::View() {

	glGenVertexArrays(1, &empty_vao);

	background_framebuffer = std::make_unique<BackgroundFramebuffer>(empty_vao);
	sprite_framebuffer = std::make_unique<SpriteFramebuffer>(empty_vao, 0);

	blit_program = Framebuffer::compile_shader_from_path(
		data_path("shaders/fullscreen_quad_v.glsl"),
		data_path("shaders/blit_f.glsl")
	);

	blit_program_ViewportOffset_uvec2 =
		glGetUniformLocation(blit_program, "ViewportOffset");


	glGenTextures(1, &sprite_tex);
	glBindTexture(GL_TEXTURE_2D, sprite_tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// generate the texture
	std::vector<glm::u8vec4> image_pixels;
	glm::uvec2 image_size;
	load_png(data_path("sprites.png"), &image_size, &image_pixels, LowerLeftOrigin);

	std::vector<uint8_t> pixels;
	pixels.reserve(image_size.x * image_size.y);

	for (size_t i = 0; i < image_size.x * image_size.y; i++) {
		pixels[i] = image_pixels[i].r;
	}

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_R8,
		image_size.x,
		image_size.y,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		pixels.data()
	);

	glBindTexture(GL_TEXTURE_2D, 0);
	GL_ERRORS();

}

View::~View() {}

void View::draw(const glm::uvec2 &drawable_size) {

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

	background_framebuffer->realloc(viewport_size);
	sprite_framebuffer->realloc(viewport_size);

	glViewport(0, 0, ScreenWidth, ScreenHeight);

	glUseProgram(background_framebuffer->program);
	glUniform1f(background_framebuffer->Time_float, total_time);
	glUseProgram(0);

	GLuint bg_tex = background_framebuffer->draw(0);

	glViewport(0, 0, viewport_size.x, viewport_size.y);

	glUseProgram(sprite_framebuffer->program);
	glUniform2ui(sprite_framebuffer->small_to_big_ViewportSize_uvec2, viewport_size.x, viewport_size.y);

	std::vector<SpriteManager::Vertex> sprite_verts;
	sprites.draw(sprite_verts);
	GLuint sprite_tex = sprite_framebuffer->draw(bg_tex, sprite_verts);

	glViewport(
		viewport_offset.x,
		viewport_offset.y,
		viewport_size.x,
		viewport_size.y
	);

	{ // draw to screen
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_BLEND);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(blit_program);

		glUniform2ui(
			blit_program_ViewportOffset_uvec2,
			viewport_offset.x,
			viewport_offset.y
		);

		glBindVertexArray(empty_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sprite_tex);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	GL_ERRORS();

}

void View::update(float elapsed) {
	total_time += elapsed;
}
