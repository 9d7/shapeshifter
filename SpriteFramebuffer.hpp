#pragma once

#include "Framebuffer.hpp"
#include <glm/glm.hpp>
#include "data_path.hpp"
#include "SpriteManager.hpp"
#include <vector>
#include "gl_errors.hpp"

class SpriteFramebuffer : public Framebuffer {
	public:

		SpriteFramebuffer(GLuint empty_vao, GLuint sprite_tex_);
		GLuint draw(GLuint old_tex, std::vector<SpriteManager::Vertex> &verts);

		GLuint small_to_big_ViewportSize_uvec2   = 0;


	protected:
		// hide old draw function
		GLuint draw(GLuint old_tex) {return 0;}

		GLuint sprite_tex;
		GLuint sprite_program;
		GLuint sprite_vao;
		GLuint sprite_vbo;


};
