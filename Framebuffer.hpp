#pragma once

#include "GL.hpp"
#include <glm/glm.hpp>
#include <string>

class Framebuffer {
public:

	Framebuffer(
		const std::string &vertex_shader_path,
		const std::string &fragment_shader_path,
		GLuint            empty_vao
	);
	virtual ~Framebuffer();

	virtual void   realloc(const glm::uvec2 &drawable_size);
	virtual GLuint draw(GLuint old_tex);

	void draw_program(GLuint program, GLuint tex_);

	static GLuint compile_shader_from_path(
		const std::string &vertex_shader_path,
		const std::string &fragment_shader_path
	);

	GLuint program = 0;
	GLuint tex     = 0;
	GLuint fbo     = 0;

protected:


	glm::uvec2 size {0, 0};

	GLuint empty_vao;

};
