#include "Renderer.hpp"
#include "GL.hpp"
#include "Load.hpp"
#include "SDL_mouse.h"
#include "Stars.hpp"
#include "data_path.hpp"
#include "gl_compile_program.hpp"
#include "gl_errors.hpp"
#include <array>
#include <cstdio>
#include <glm/glm.hpp>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "load_save_png.hpp"
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>

Load < GLuint > texture_program(LoadTagDefault, []() -> GLuint *{

	std::ifstream vfs(data_path("shaders/texture_vertex.glsl"));
	std::string vertex_shader(
		(std::istreambuf_iterator<char>(vfs)),
		(std::istreambuf_iterator<char>())
	);

	std::ifstream ffs(data_path("shaders/texture_fragment.glsl"));
	std::string fragment_shader(
		(std::istreambuf_iterator<char>(ffs)),
		(std::istreambuf_iterator<char>())
	);

	GLuint program = gl_compile_program(
		vertex_shader,
		fragment_shader
	);

	GLuint TEX_sampler2D = glGetUniformLocation(program, "TEX");

	//set TEX to always refer to texture binding zero:
	glUseProgram(program); //bind program -- glUniform* calls refer to this program now
	glUniform1i(TEX_sampler2D, 0); //set TEX to sample from GL_TEXTURE0
	glUseProgram(0); //unbind program -- glUniform* calls refer to ??? now

	return new GLuint(program);

});

struct BackgroundProgram {
	GLuint program;
	GLint Time_float;
	GLint Camera_ivec2;
};

// program that simply draws the background.
Load < BackgroundProgram > background_program(LoadTagDefault, []() -> BackgroundProgram *{

	std::ifstream vfs(data_path("shaders/background_vertex.glsl"));
	std::string vertex_shader(
		(std::istreambuf_iterator<char>(vfs)),
		(std::istreambuf_iterator<char>())
	);

	std::ifstream ffs(data_path("shaders/background_fragment.glsl"));
	std::string fragment_shader(
		(std::istreambuf_iterator<char>(ffs)),
		(std::istreambuf_iterator<char>())
	);

	GLuint program = gl_compile_program(
		vertex_shader,
		fragment_shader
	);

	return new BackgroundProgram {
		program,
		glGetUniformLocation(program, "Time"),
		glGetUniformLocation(program, "Camera")
	};

});

struct SpriteSmallProgram {
	GLuint program;
	GLint ScreenToClip_mat4;
};

Load < SpriteSmallProgram > sprite_small_program(LoadTagDefault, []() -> SpriteSmallProgram *{

	std::ifstream vfs(data_path("shaders/sprite_small_vertex.glsl"));
	std::string vertex_shader(
		(std::istreambuf_iterator<char>(vfs)),
		(std::istreambuf_iterator<char>())
	);

	std::ifstream ffs(data_path("shaders/sprite_small_fragment.glsl"));
	std::string fragment_shader(
		(std::istreambuf_iterator<char>(ffs)),
		(std::istreambuf_iterator<char>())
	);

	GLuint program = gl_compile_program(
		vertex_shader,
		fragment_shader
	);

	GLuint TEX_sampler2D = glGetUniformLocation(program, "TEX");

	//set TEX to always refer to texture binding zero:
	glUseProgram(program); //bind program -- glUniform* calls refer to this program now
	glUniform1i(TEX_sampler2D, 0); //set TEX to sample from GL_TEXTURE0
	glUseProgram(0); //unbind program -- glUniform* calls refer to ??? now

	return new SpriteSmallProgram {
		program,
		glGetUniformLocation(program, "ScreenToClip")
	};

});

struct StarProgram {
	GLuint program;
	std::array<GLint, NUM_INSTANCES> TexCoords_vec2;
	std::array<GLint, NUM_INSTANCES> Colors_vec4;
	std::array<GLint, NUM_INSTANCES> Positions_ivec2;
	std::array<GLint, NUM_INSTANCES> Parallax_float;
	GLint Camera_ivec2;
	GLint ScreenToClip_mat4;
	GLint AtlasSize_uvec2;
};

Load < StarProgram > star_program(LoadTagDefault, []() -> StarProgram *{

	std::ifstream vfs(data_path("shaders/star_vertex.glsl"));
	std::string vertex_shader(
		(std::istreambuf_iterator<char>(vfs)),
		(std::istreambuf_iterator<char>())
	);

	std::ifstream ffs(data_path("shaders/star_fragment.glsl"));
	std::string fragment_shader(
		(std::istreambuf_iterator<char>(ffs)),
		(std::istreambuf_iterator<char>())
	);

	GLuint program = gl_compile_program(
		vertex_shader,
		fragment_shader
	);

	GLuint TEX_sampler2D = glGetUniformLocation(program, "TEX");

	//set TEX to always refer to texture binding zero:
	glUseProgram(program); //bind program -- glUniform* calls refer to this program now
	glUniform1i(TEX_sampler2D, 0); //set TEX to sample from GL_TEXTURE0
	glUseProgram(0); //unbind program -- glUniform* calls refer to ??? now

	std::array<GLint, NUM_INSTANCES> TexCoords_vec2;
	std::array<GLint, NUM_INSTANCES> Colors_vec4;
	std::array<GLint, NUM_INSTANCES> Positions_ivec2;
	std::array<GLint, NUM_INSTANCES> Parallax_float;

	for (size_t i = 0; i < NUM_INSTANCES; i++) {
		TexCoords_vec2[i]  = glGetUniformLocation(program, ("TexCoords[" + std::to_string(i) + "]").c_str());
		Colors_vec4[i]     = glGetUniformLocation(program, ("Colors["    + std::to_string(i) + "]").c_str());
		Positions_ivec2[i] = glGetUniformLocation(program, ("Positions[" + std::to_string(i) + "]").c_str());
		Parallax_float[i]  = glGetUniformLocation(program, ("Parallax["  + std::to_string(i) + "]").c_str());
	}

	return new StarProgram {
		program,
		TexCoords_vec2,
		Colors_vec4,
		Positions_ivec2,
		Parallax_float,
		glGetUniformLocation(program, "Camera"),
		glGetUniformLocation(program, "ScreenToClip"),
		glGetUniformLocation(program, "AtlasSize")
	};

});

float quad_verts[] = {
	-1.0f,  1.0f,  0.0f,  1.0f,
	-1.0f, -1.0f,  0.0f,  0.0f,
	 1.0f, -1.0f,  1.0f,  0.0f,
	-1.0f,  1.0f,  0.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,  0.0f,
	 1.0f,  1.0f,  1.0f,  1.0f
};

float star_verts[] = {
	0.0f, 8.0f,
	0.0f, 0.0f,
	8.0f, 0.0f,
	0.0f, 8.0f,
	8.0f, 0.0f,
	8.0f, 8.0f
};

Renderer::Renderer() {

	// construct OpenGL stuff

	auto make_fb = [](
		GLuint *fbo,
		GLuint *tex,
		GLuint *rbo,
		GLint  scaling_type,
		bool   depth,
		GLuint x,
		GLuint y
	) {

		// framebuffer
		glGenFramebuffers(1, fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

		// texture
		glGenTextures(1, tex);
		glBindTexture(GL_TEXTURE_2D, *tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, scaling_type); // TODO experiment with GL_NEAREST
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, scaling_type); // TODO experiment with GL_NEAREST
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex, 0);

		if (depth) {
			// renderbuffer (for depth/stencil)
			glGenRenderbuffers(1, rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, x, y);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *rbo);
		}


		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		GL_ERRORS();

	};

	auto make_vb = [](
		GLuint *vbo,
		GLuint *vao,
		const std::vector<size_t> &sizes,
		size_t total_size
	) {

		glGenVertexArrays(1, vao);
		glGenBuffers(1, vbo);

		GLuint offset = 0;

		glBindVertexArray(*vao);
		glBindBuffer(GL_ARRAY_BUFFER, *vbo);

		for (size_t i = 0; i < sizes.size(); i++) {

			glVertexAttribPointer(
				(GLuint)i,
				(GLint)sizes[i],
				GL_FLOAT,
				GL_FALSE,
				(GLuint)(total_size * sizeof(float)),
				(GLbyte *)0 + offset * sizeof(float)
			);
			glEnableVertexAttribArray((GLuint)i);
			offset += (GLuint)sizes[i];

		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		GL_ERRORS();

	};

	make_fb(&tiny_fbo, &tiny_tex, &tiny_rbo, GL_NEAREST, true, ScreenWidth, ScreenHeight);
	make_vb(&quad_vbo, &quad_vao, {2, 2}, 4);

	// load single quad onto vb
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(quad_verts),
		quad_verts,
		GL_STATIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	make_vb(&tiny_vbo, &tiny_vao, {2, 2, 4}, 8);

	make_vb(&star_vbo, &star_vao, {2}, 2);

	glBindBuffer(GL_ARRAY_BUFFER, star_vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(star_verts),
		star_verts,
		GL_STATIC_DRAW
	);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// make texture atlas
	std::vector<glm::u8vec4> atlas_data;

	load_png(
		data_path("sprites.png"),
		&atlas_size,
		&atlas_data,
		OriginLocation::LowerLeftOrigin
	);

	glGenTextures(1, &atlas_tex);
	glBindTexture(GL_TEXTURE_2D, atlas_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas_size.x, atlas_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas_data.data());
	glBindTexture(GL_TEXTURE_2D, 0);

}

Renderer::~Renderer() {}

void Renderer::update_cursor_position(const glm::vec2 &position) {
	cursor_position = position;
}

void Renderer::draw(const glm::uvec2 &drawable_size) {

	small_verts.clear();

	auto draw_rect = [](
		std::vector<Renderer::Vertex> &verts,
		const glm::uvec2 &atlas_size,
		const glm::ivec2 &location,
		float             rotation,
		const glm::uvec2 &tex,
		const glm::uvec2 &size,
		const glm::u8vec4 &color
	) {

		glm::vec2 p0 = glm::vec2(-0.5f, -0.5f);
		glm::vec2 p1 = glm::vec2(-0.5f,  0.5f);
		glm::vec2 p2 = glm::vec2( 0.5f,  0.5f);
		glm::vec2 p3 = glm::vec2( 0.5f, -0.5f);

		glm::mat2 rot = glm::mat2x2(
			glm::vec2(glm::cos(rotation), glm::sin(rotation)),
			glm::vec2(-glm::sin(rotation), glm::cos(rotation))
		);

		p0 *= size;
		p1 *= size;
		p2 *= size;
		p3 *= size;

		p0 = rot * p0;
		p1 = rot * p1;
		p2 = rot * p2;
		p3 = rot * p3;

		p0 += glm::vec2(location);
		p1 += glm::vec2(location);
		p2 += glm::vec2(location);
		p3 += glm::vec2(location);

		glm::vec2 t_min = glm::vec2(tex) / glm::vec2(atlas_size);
		glm::vec2 t_max = glm::vec2(tex + size) / glm::vec2(atlas_size);

		glm::vec4 c = glm::vec4(color) / 255.0f;

		verts.push_back(Vertex {
			p1,
			glm::vec2(t_min.x, t_max.y),
			c
		});

		verts.push_back(Vertex {
			p0,
			glm::vec2(t_min.x, t_min.y),
			c
		});

		verts.push_back(Vertex {
			p3,
			glm::vec2(t_max.x, t_min.y),
			c
		});

		verts.push_back(Vertex {
			p1,
			glm::vec2(t_min.x, t_max.y),
			c
		});

		verts.push_back(Vertex {
			p3,
			glm::vec2(t_max.x, t_min.y),
			c
		});

		verts.push_back(Vertex {
			p2,
			glm::vec2(t_max.x, t_max.y),
			c
		});

	};

	{ // first pass: draw background

		// first pass
		glBindFramebuffer(GL_FRAMEBUFFER, tiny_fbo);
		glViewport(0, 0, ScreenWidth, ScreenHeight);
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw background
		glDisable(GL_DEPTH_TEST);
		glUseProgram(background_program->program);

		glUniform1f(background_program->Time_float, total_elapsed);
		glUniform2i(background_program->Camera_ivec2, (int)camera_position.x, (int)camera_position.y);
		glBindVertexArray(quad_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glUseProgram(0);

	}

	{ // second pass: draw stars

		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		GL_ERRORS();

		glUseProgram(star_program->program);

		glUniform2i(star_program->Camera_ivec2, (int)camera_position.x, (int)camera_position.y);
		glUniformMatrix4fv(
			star_program->ScreenToClip_mat4,
			1,
			GL_FALSE,
			glm::value_ptr(glm::ortho(0.0f, (float)ScreenWidth, 0.0f, (float)ScreenHeight))
		);
		glUniform2ui(star_program->AtlasSize_uvec2, atlas_size.x, atlas_size.y);

		stars.draw(
			star_program->TexCoords_vec2,
			star_program->Colors_vec4,
			star_program->Positions_ivec2,
			star_program->Parallax_float
		);

		glBindVertexArray(star_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, atlas_tex);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, NUM_INSTANCES);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glUseProgram(0);
		GL_ERRORS();

	}

// currently, we have no use for this--but i suspect we will
//#define TINY_ABSOLUTE
#ifdef TINY_ABSOLUTE
	{ // third pass: draw absolute position on tiny screen
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glBindBuffer(GL_ARRAY_BUFFER, tiny_vbo);
		glBufferData(
			GL_ARRAY_BUFFER,
			small_verts.size() * sizeof(Vertex),
			small_verts.data(),
			GL_DYNAMIC_DRAW
		);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glUseProgram(sprite_small_program->program);

		glUniformMatrix4fv(
			sprite_small_program->ScreenToClip_mat4,
			1,
			GL_FALSE,
			glm::value_ptr(
				glm::ortho(
					0.0f,
					(float)ScreenWidth,
					0.0f,
					(float)ScreenHeight
				) *
				glm::mat4(
					glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
					glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
					glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
					glm::vec4((float)(-(int)(camera_position.x)), (float)(-(int)(camera_position.y)), 0.0f, 1.0f)
				)
			)
		);

		glBindVertexArray(tiny_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, atlas_tex);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)small_verts.size());

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
#endif

	glm::uvec2 actual_drawable_size;
	{ // draw to actual screen
		// clear entire screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, drawable_size.x, drawable_size.y);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		// get real drawable size
		size_t real_drawable_scale = std::min(
			drawable_size.x * ScreenHeight,
			drawable_size.y * ScreenWidth
		);
		actual_drawable_size = glm::uvec2(
			real_drawable_scale / ScreenHeight,
			real_drawable_scale / ScreenWidth
		);
		glViewport(
			(drawable_size.x - actual_drawable_size.x) / 2,
			(drawable_size.y - actual_drawable_size.y) / 2,
			actual_drawable_size.x,
			actual_drawable_size.y
		);

		glUseProgram(*texture_program);
		glBindVertexArray(quad_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tiny_tex);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glUseProgram(0);

	}

	{ // relative position -- all enemies, bullets, etc. get drawn here

		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		small_verts.clear();

		for (Bullet_ &b : bullets) {
			glm::uvec2 frame0;
			glm::uvec2 frame1;

			if (b.color == BulletColor::Blue) {
				frame0 = {48, 16};
				frame1 = {48, 24};
			} else if (b.color == BulletColor::Red) {
				frame0 = {40, 16};
				frame1 = {40, 24};
			} else if (b.color == BulletColor::Purple) {
				frame0 = {56, 16};
				frame1 = {56, 24};
			} else {
				throw std::runtime_error("Undefined BulletColor");
			}

			static constexpr float ANIM_SPEED = 0.2f;
			glm::uvec2 frame = frame0;
			if (glm::mod(b.elapsed, ANIM_SPEED) > (ANIM_SPEED / 2.0f)) {
				frame = frame1;
			}

			draw_rect(
				small_verts,
				atlas_size,
				b.position,
				0.0f,
				frame,
				glm::uvec2(8, 8),
				glm::u8vec4(255, 255, 255, 255)
			);

		}

		for (Enemy_ &e : enemies) {
			glm::uvec2 size;
			glm::uvec2 blue;
			glm::uvec2 red;

			if (e.type == EnemyType::Hunter) {
				size = {8, 8};
				blue = {32, 24};
				red = {32, 16};
			} else if (e.type == EnemyType::Soldier) {
				size = {16, 16};
				blue = {32, 32};
				red = {48, 32};
			} else if (e.type == EnemyType::Turret) {
				size = {16, 16};
				blue = {0, 32};
				red = {16, 32};
			} else {
				throw std::runtime_error("EnemyType not found");
			}

			glm::uvec2 tex;
			if (e.color == BulletColor::Blue) {
				tex = blue;
			} else if (e.color == BulletColor::Red) {
				tex = red;
			} else {
				throw std::runtime_error("Color not supported on enemy");
			}

			draw_rect(
				small_verts,
				atlas_size,
				e.position,
				e.rotation,
				tex,
				size,
				glm::u8vec4(255, 255, 255, 255)
			);

		}

		// draw player
		glm::uvec2 tex;
		if (char_color == BulletColor::Blue) {
			tex = { 0, 16};
		} else if (char_color == BulletColor::Red) {
			tex = {16, 16};
		} else {
			throw std::runtime_error("Color not supported for player");
		}
		draw_rect(
			small_verts,
			atlas_size,
			player_position,
			char_rotation - 1.571f,
			tex,
			glm::uvec2(16, 16),
			glm::u8vec4(255, 255, 255, 255)
		);

		glBindBuffer(GL_ARRAY_BUFFER, tiny_vbo);
		glBufferData(
			GL_ARRAY_BUFFER,
			small_verts.size() * sizeof(Vertex),
			small_verts.data(),
			GL_DYNAMIC_DRAW
		);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glUseProgram(sprite_small_program->program);

		glUniformMatrix4fv(
			sprite_small_program->ScreenToClip_mat4,
			1,
			GL_FALSE,
			glm::value_ptr(
				glm::ortho(
					0.0f,
					(float)ScreenWidth,
					0.0f,
					(float)ScreenHeight
				) *
				glm::mat4(
					glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
					glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
					glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
					glm::vec4((float)(-(int)(camera_position.x)), (float)(-(int)(camera_position.y)), 0.0f, 1.0f)
				)
			)
		);

		glBindVertexArray(tiny_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, atlas_tex);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)small_verts.size());

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glUseProgram(0);

	}

	{ // absolute position

		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		int x, y;
		SDL_GetMouseState(&x, &y);
		y = drawable_size.y - y;

		small_verts.clear();

		// draw mouse
		draw_rect(
			small_verts,
			atlas_size,
			cursor_position,
			0.0f,
			glm::uvec2(32, 8),
			glm::uvec2(8, 8),
			glm::u8vec4(255, 255, 255, 255)
		);

		glBindBuffer(GL_ARRAY_BUFFER, tiny_vbo);
		glBufferData(
			GL_ARRAY_BUFFER,
			small_verts.size() * sizeof(Vertex),
			small_verts.data(),
			GL_DYNAMIC_DRAW
		);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glUseProgram(sprite_small_program->program);

		glUniformMatrix4fv(
			sprite_small_program->ScreenToClip_mat4,
			1,
			GL_FALSE,
			glm::value_ptr(
				glm::ortho(
					0.0f,
					(float)ScreenWidth,
					0.0f,
					(float)ScreenHeight
				)
			)
		);

		glBindVertexArray(tiny_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, atlas_tex);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)small_verts.size());

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glUseProgram(0);

	}
	GL_ERRORS();

}

void Renderer::update(float elapsed) {
	total_elapsed += elapsed;
	stars.update(elapsed);

	for (Bullet_ &b : bullets) {
		b.elapsed += elapsed;
	}
}

void Renderer::update_camera_position(const glm::vec2 &position) {
	camera_position = position - glm::vec2(ScreenWidth, ScreenHeight) / 2.0f;
}

void Renderer::update_char_position(const glm::vec2 &position, float rotation) {
	player_position = position;
	char_rotation = rotation;
}

void Renderer::update_char_color(BulletColor color) {
	char_color = color;
}

Renderer::Enemy Renderer::new_enemy(const glm::vec2 &position, float rotation, EnemyType type, BulletColor color) {
	enemies.push_front(Renderer::Enemy_ {
		position,
		rotation,
		0.0f,
		type,
		color
	});
	return enemies.begin();
}

void Renderer::update_enemy_position(Enemy e, const glm::vec2 &position) {
	e->position = position;
}

void Renderer::update_enemy_rotation(Enemy e, float rotation) {
	e->rotation = rotation;
}

void Renderer::update_enemy_color(Enemy e, BulletColor color) {
	e->color = color;
}

void Renderer::invuln_enemy(Enemy e) {
	static constexpr float INVULN_TIME = 0.2f;
	e->invuln_time = INVULN_TIME;
}

void Renderer::destroy_enemy(Enemy e) {
	enemies.erase(e);
}

Renderer::Bullet Renderer::new_bullet(const glm::vec2 &position, BulletColor color) {
	bullets.push_front(Renderer::Bullet_ {
		position,
		color
	});
	return bullets.begin();
}

void Renderer::update_bullet_position(Bullet b, const glm::vec2 &position) {
	b->position = position;
}

void Renderer::update_bullet_color(Bullet b, BulletColor color) {
	b->color = color;
}

void Renderer::destroy_bullet(Bullet b) {
	bullets.erase(b);

