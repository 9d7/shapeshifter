#include "View.hpp"
#include "SpriteFramebuffer.hpp"
#include "GL.hpp"

SpriteFramebuffer::SpriteFramebuffer(GLuint empty_vao, GLuint sprite_tex_)
	: Framebuffer(
		data_path("shaders/fullscreen_quad_v.glsl"),
		data_path("shaders/small_to_big_f.glsl"),
		empty_vao
	), sprite_tex(sprite_tex_) {

	small_to_big_ViewportSize_uvec2 =
		glGetUniformLocation(program, "ViewportSize");

	GLint small_to_big_TextureSize_uvec2 =
		glGetUniformLocation(program, "TextureSize");

	glUseProgram(program);
	glUniform2ui(small_to_big_TextureSize_uvec2, View::ScreenWidth, View::ScreenHeight);
	glUseProgram(0);

	// sprite program
	sprite_program = compile_shader_from_path(
		data_path("shaders/sprites_v.glsl"),
		data_path("shaders/sprites_f.glsl")
	);

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

	GL_ERRORS();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GL_ERRORS();
}

GLuint SpriteFramebuffer::draw(GLuint old_tex, std::vector<SpriteManager::Vertex> &verts) {

	Framebuffer::draw(old_tex);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);

	glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		verts.size() * sizeof(SpriteManager::Vertex),
		verts.data(),
		GL_DYNAMIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glUseProgram(sprite_program);
	glBindVertexArray(sprite_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sprite_tex);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)verts.size());

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GL_ERRORS();
	return tex;
}

