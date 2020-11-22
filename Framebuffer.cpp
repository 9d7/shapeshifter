#include "Framebuffer.hpp"
#include "GL.hpp"
#include "gl_compile_program.hpp"
#include "gl_errors.hpp"
#include <fstream>
#include <iterator>

GLuint Framebuffer::compile_shader_from_path(
	const std::string &vertex_shader_path,
	const std::string &fragment_shader_path
) {

	std::ifstream vfs(vertex_shader_path);
	std::string vertex_shader(
		(std::istreambuf_iterator<char>(vfs)),
		(std::istreambuf_iterator<char>())
	);

	std::ifstream ffs(fragment_shader_path);
	std::string fragment_shader(
		(std::istreambuf_iterator<char>(ffs)),
		(std::istreambuf_iterator<char>())
	);

	GLuint program_ = gl_compile_program(vertex_shader, fragment_shader);

	GLint TEX_sampler2D = glGetUniformLocation(program_, "TEX");

	if (TEX_sampler2D != -1) {
		// set TEX to always refer to texture binding zero
		glUseProgram(program_);
		glUniform1i(TEX_sampler2D, 0);
		glUseProgram(0);
	}

	return program_;

}

Framebuffer::Framebuffer(
	const std::string &vertex_shader_path,
	const std::string &fragment_shader_path,
	GLuint            empty_vao
) : empty_vao(empty_vao) {

	program = compile_shader_from_path(
		vertex_shader_path,
		fragment_shader_path
	);

}

Framebuffer::~Framebuffer() {
	glDeleteTextures(1, &tex);
	glDeleteFramebuffers(1, &fbo);
	glDeleteProgram(program);
}

void Framebuffer::realloc(const glm::uvec2 &drawable_size) {

	if (drawable_size == size) return;
	size = drawable_size;

	if (tex == 0) {
		glGenTextures(1, &tex);
	}

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB16F, // 15-466 example code uses half-floats...why?
		size.x,
		size.y,
		0,
		GL_RGB,
		GL_FLOAT,
		NULL
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (fbo == 0) {
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D,
			tex,
			0
		);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GL_ERRORS();

}

void Framebuffer::draw_program(GLuint program, GLuint tex_) {

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);
	glBindVertexArray(empty_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GL_ERRORS();


}

GLuint Framebuffer::draw(GLuint old_tex) {

	draw_program(program, old_tex);
	return tex;

}
