#include "Animation.hpp"
#include "View.hpp"
#include "SpriteFramebuffer.hpp"
#include "GL.hpp"
#include "data_path.hpp"
#include <chrono>

SpriteFramebuffer::SpriteFramebuffer(GLuint empty_vao, GLuint sprite_tex_)
	: Framebuffer(
		data_path("shaders/fullscreen_quad_v.glsl"),
		data_path("shaders/small_to_big_f.glsl"),
		empty_vao
	), sprite_tex(sprite_tex_) {

	small_to_big_Camera_vec2 =
		glGetUniformLocation(program, "Camera");

	small_to_big_ViewportSize_uvec2 =
		glGetUniformLocation(program, "ViewportSize");

	small_to_big_Parallax_float =
		glGetUniformLocation(program, "Parallax");

	GLint small_to_big_TextureSize_uvec2 =
		glGetUniformLocation(program, "TextureSize");

	glUseProgram(program);
	glUniform2ui(small_to_big_TextureSize_uvec2, View::ScreenWidth, View::ScreenHeight);
	glUseProgram(0);

	// star program
	star_program = compile_shader_from_path(
		data_path("shaders/bg_star_v.glsl"),
		data_path("shaders/bg_star_f.glsl")
	);

	GLint star_ScreenSize_ivec2 = glGetUniformLocation(star_program, "ScreenSize");
	star_Camera_vec2 = glGetUniformLocation(star_program, "Camera");
	star_ViewportSize_ivec2 = glGetUniformLocation(star_program, "ViewportSize");
	GLint star_NumStars_uint = glGetUniformLocation(star_program, "NumStars");
	star_TexCoords_vec2v =
		glGetUniformLocation(star_program, "TexCoords");
	GL_ERRORS();
	glGetUniformuiv(star_program, star_NumStars_uint, &num_stars);
	GL_ERRORS();
	glUseProgram(star_program);
	glUniform2i(star_ScreenSize_ivec2, View::ScreenWidth, View::ScreenHeight);
	glUseProgram(0);

	// screen edge program
	screen_edge_program = compile_shader_from_path(
		data_path("shaders/screen_edge_v.glsl"),
		data_path("shaders/screen_edge_f.glsl")
	);
	GLint screen_edge_FieldSize_uvec2 = glGetUniformLocation(screen_edge_program, "FieldSize");
	GLint screen_edge_ScreenSize_uvec2 = glGetUniformLocation(screen_edge_program, "ScreenSize");

	glUseProgram(screen_edge_program);
	glUniform2ui(screen_edge_FieldSize_uvec2, View::FieldWidth, View::FieldHeight);
	glUniform2ui(screen_edge_ScreenSize_uvec2, View::ScreenWidth, View::ScreenHeight);
	glUseProgram(0);

	// sprite program
	sprite_program = compile_shader_from_path(
		data_path("shaders/sprites_v.glsl"),
		data_path("shaders/sprites_f.glsl")
	);

	sprite_Camera_vec2 =
		glGetUniformLocation(sprite_program, "Camera");

	sprite_ViewportSize_vec2 =
		glGetUniformLocation(sprite_program, "ViewportSize");
	glUseProgram(sprite_program);
	glUniform2f(glGetUniformLocation(sprite_program, "ScreenSize"), View::ScreenWidth, View::ScreenHeight);
	glUseProgram(0);

	glGenVertexArrays(1, &sprite_vao);
	glGenBuffers(1, &sprite_vbo);

	glBindVertexArray(sprite_vao);
	glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo);
	GLuint Position_vec2 = glGetAttribLocation(sprite_program, "Position");
	glVertexAttribPointer(
		Position_vec2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(SpriteManager::Vertex),
		(GLbyte *)0
	);
	glEnableVertexAttribArray(Position_vec2);
	GLuint Rotation_float = glGetAttribLocation(sprite_program, "Rotation");
	glVertexAttribPointer(
		Rotation_float,
		1,
		GL_FLOAT,
		GL_FALSE,
		sizeof(SpriteManager::Vertex),
		(GLbyte *)0 + 4*2
	);
	glEnableVertexAttribArray(Rotation_float);
	GLuint Size_vec2 = glGetAttribLocation(sprite_program, "Size");
	glVertexAttribPointer(
		Size_vec2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(SpriteManager::Vertex),
		(GLbyte *)0 + 4*2 + 4*1
	);
	glEnableVertexAttribArray(Size_vec2);
	GLuint TexCoords_vec2 = 3;
	glVertexAttribPointer(
		TexCoords_vec2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(SpriteManager::Vertex),
		(GLbyte *)0 + 4*2 + 4*1 + 4*2
	);
	glEnableVertexAttribArray(TexCoords_vec2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GL_ERRORS();


	// set up star stuff

}

GLuint SpriteFramebuffer::draw(
	GLuint old_tex,
	std::vector<SpriteManager::Vertex> &sprite_verts,
	std::vector<SpriteManager::Vertex> &ui_field_verts,
	std::vector<SpriteManager::Vertex> &ui_border_verts
) {

	Framebuffer::draw(old_tex);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// star program
	glUseProgram(star_program);
	glBindVertexArray(empty_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sprite_tex);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, num_stars);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		sprite_verts.size() * sizeof(SpriteManager::Vertex),
		sprite_verts.data(),
		GL_DYNAMIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// sprite program
	glUseProgram(sprite_program);
	glBindVertexArray(sprite_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sprite_tex);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)sprite_verts.size());

	glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		ui_field_verts.size() * sizeof(SpriteManager::Vertex),
		ui_field_verts.data(),
		GL_DYNAMIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUniform2f(sprite_Camera_vec2, ((float)View::ScreenWidth)/2.0f, ((float)View::ScreenHeight)/2.0f);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)ui_field_verts.size());

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	// screen edge program
	glUseProgram(screen_edge_program);
	glBindVertexArray(empty_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sprite_tex);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 2);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		ui_border_verts.size() * sizeof(SpriteManager::Vertex),
		ui_border_verts.data(),
		GL_DYNAMIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(sprite_program);
	glBindVertexArray(sprite_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sprite_tex);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)ui_border_verts.size());

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GL_ERRORS();
	return tex;

}

