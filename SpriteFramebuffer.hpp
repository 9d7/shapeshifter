#pragma once

#include "Animation.hpp"
#include "Framebuffer.hpp"
#include <glm/glm.hpp>
#include "data_path.hpp"
#include "SpriteManager.hpp"
#include <random>
#include <vector>
#include "gl_errors.hpp"

class SpriteFramebuffer : public Framebuffer {
	public:

		SpriteFramebuffer(GLuint empty_vao, GLuint sprite_tex_);
		GLuint draw(GLuint old_tex, std::vector<SpriteManager::Vertex> &verts);

		GLuint small_to_big_Camera_vec2          = 0;
		GLuint small_to_big_ViewportSize_uvec2   = 0;
		GLuint small_to_big_Parallax_float       = 0;
		GLuint sprite_Camera_vec2                = 0;
		GLuint star_Camera_vec2                  = 0;
		GLuint star_TexCoords_vec2v              = 0;

		GLuint sprite_program;
		GLuint star_program;

		uint32_t num_stars;

	protected:

		// hide old draw function
		GLuint draw(GLuint old_tex) {return 0;}

		GLuint sprite_tex;
		GLuint sprite_vao;
		GLuint sprite_vbo;



};
