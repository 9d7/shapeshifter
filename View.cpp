#include "View.hpp"
#include "GL.hpp"
#include "SpriteManager.hpp"
#include "Ui.hpp"
#include "data_path.hpp"
#include "glm/fwd.hpp"
#include "load_save_png.hpp"
#include <cstdio>
#include <memory>

View::View() {

	sprites = std::make_shared<SpriteManager>();
	ui = std::make_shared<Ui>();

	glGenVertexArrays(1, &empty_vao);

	background_framebuffer = std::make_unique<BackgroundFramebuffer>(empty_vao);

	static constexpr float CLOUD_PARALLAX = 0.6f;
	glUseProgram(background_framebuffer->program);
	glUniform1f(background_framebuffer->Parallax_float, CLOUD_PARALLAX);
	glUseProgram(0);

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

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		image_size.x,
		image_size.y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image_pixels.data()
	);

	glBindTexture(GL_TEXTURE_2D, 0);
	GL_ERRORS();

	sprite_framebuffer = std::make_unique<SpriteFramebuffer>(empty_vao, sprite_tex);
	stars = std::make_unique<Stars>(sprite_framebuffer->num_stars);

	glUseProgram(sprite_framebuffer->program);
	glUniform1f(sprite_framebuffer->small_to_big_Parallax_float, CLOUD_PARALLAX);
	glUseProgram(0);
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

	glViewport(0, 0, FieldWidth + 1, FieldHeight + 1);

	glUseProgram(background_framebuffer->program);
	glUniform1f(background_framebuffer->Time_float, total_time);
	glUniform2f(
		background_framebuffer->Camera_vec2,
		camera_position.x,
		camera_position.y
	);

	GLuint bg_tex = background_framebuffer->draw(0);

	glViewport(0, 0, viewport_size.x, viewport_size.y);

	glUseProgram(sprite_framebuffer->program);
	glUniform2ui(sprite_framebuffer->small_to_big_ViewportSize_uvec2, viewport_size.x, viewport_size.y);
	glUniform2f(
		sprite_framebuffer->small_to_big_Camera_vec2,
		camera_position.x,
		camera_position.y
	);

	glUseProgram(sprite_framebuffer->star_program);
	glUniform2f(
		sprite_framebuffer->star_Camera_vec2,
		camera_position.x,
		camera_position.y
	);
	glUniform2fv(
		sprite_framebuffer->star_TexCoords_vec2v,
		sprite_framebuffer->num_stars,
		(GLfloat *)(stars->star_tex_coords.data())
	);
	glUniform2i(
		sprite_framebuffer->star_ViewportSize_ivec2,
		(int)viewport_size.x,
		(int)viewport_size.y
	);
	glUseProgram(0);

	glUseProgram(sprite_framebuffer->sprite_program);
	glUniform2f(sprite_framebuffer->sprite_Camera_vec2, camera_position.x, camera_position.y);
	glUniform2f(sprite_framebuffer->sprite_ViewportSize_vec2, (float)drawable_size.x, (float)drawable_size.y);

	std::vector<SpriteManager::Vertex> sprite_verts;
	sprites->draw(sprite_verts);

	std::vector<SpriteManager::Vertex> ui_field_verts;
	std::vector<SpriteManager::Vertex> ui_border_verts;

	ui->draw_field(ui_field_verts);
	if (draw_ui) ui->draw_border(ui_border_verts);

	GLuint sprite_tex = sprite_framebuffer->draw(bg_tex, sprite_verts, ui_field_verts, ui_border_verts);

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
	sprites->update(elapsed);
	stars->update(elapsed);
	ui->update(elapsed);
}

void View::update_camera(const glm::vec2 &pos) {
	camera_position = pos + glm::vec2(ScreenWidth - FieldWidth, ScreenHeight - FieldHeight) / 2.0f;
}

glm::vec2 View::get_camera_position() {
	return camera_position;
}
