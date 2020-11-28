#pragma once

#include <glm/glm.hpp>
#include "Framebuffer.hpp"
#include "GL.hpp"
#include "SpriteManager.hpp"
#include "data_path.hpp"
#include "gl_compile_program.hpp"
#include "gl_errors.hpp"
#include "glm/fwd.hpp"
#include "load_save_png.hpp"
#include <fstream>
#include <iterator>
#include <memory>
#include <vector>

class View {

	public:

		View();
		~View();

		void draw(const glm::uvec2 &drawable_size);
		void update(float elapsed);

		enum : size_t {
			ScreenWidth  = 320,
			ScreenHeight = 240
		};

		SpriteManager sprites {};

	private:

		class BackgroundFramebuffer : public Framebuffer {

			public:

				BackgroundFramebuffer(GLuint empty_vao)
				: Framebuffer(
					data_path("shaders/fullscreen_quad_v.glsl"),
					data_path("shaders/bg_cloud_f.glsl"),
					empty_vao) {

					star_program = compile_shader_from_path(
						data_path("shaders/bg_star_v.glsl"),
						data_path("shaders/bg_star_f.glsl")
					);

					Time_float = glGetUniformLocation(program, "Time");

					// generate blue noise texture
					glGenTextures(1, &blue_noise_tex);
					glBindTexture(GL_TEXTURE_2D, blue_noise_tex);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

					// generate the texture
					std::vector<glm::u8vec4> image_pixels;
					glm::uvec2 image_size;
					load_png(data_path("blue_noise.png"), &image_size, &image_pixels, LowerLeftOrigin);

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

				GLuint draw(GLuint old_tex) override {

					// draw clouds, which don't need texture
					Framebuffer::draw(blue_noise_tex);

					glDisable(GL_DEPTH_TEST);
					glDisable(GL_STENCIL_TEST);
					glEnable(GL_BLEND);

					glBindFramebuffer(GL_FRAMEBUFFER, fbo);
					glUseProgram(star_program);
					glBindVertexArray(empty_vao);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, old_tex);

					// TODO
					glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);

					glBindTexture(GL_TEXTURE_2D, 0);
					glBindVertexArray(0);
					glUseProgram(0);
					glBindFramebuffer(GL_FRAMEBUFFER, 0);

					GL_ERRORS();

					return tex;

				}

				void realloc(const glm::uvec2 &drawable_size) override {
					Framebuffer::realloc(glm::uvec2(ScreenWidth, ScreenHeight));
				}

				GLint Time_float = 0;

			private:
				GLuint blue_noise_tex = 0;
				GLuint star_program = 0;
		};

		class SpriteFramebuffer : public Framebuffer {
			public:

				SpriteFramebuffer(GLuint empty_vao, GLuint sprite_tex_)
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
					glUniform2ui(small_to_big_TextureSize_uvec2, ScreenWidth, ScreenHeight);
					glUseProgram(0);

					// sprite program
					sprite_program = compile_shader_from_path(
						data_path("shaders/sprites_v.glsl"),
						data_path("shaders/sprites_f.glsl")
					);

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

					GLuint Size_uvec2 = glGetAttribLocation(sprite_program, "Size");
					glVertexAttribPointer(
						Size_uvec2,
						2,
						GL_UNSIGNED_INT,
						GL_FALSE,
						sizeof(SpriteManager::Vertex),
						(GLbyte *)0 + 4*2 + 4*1
					);
					glEnableVertexAttribArray(Size_uvec2);

					GLuint Texture_uvec2 = glGetAttribLocation(sprite_program, "Texture");
					glVertexAttribPointer(
						Texture_uvec2,
						2,
						GL_UNSIGNED_INT,
						GL_FALSE,
						sizeof(SpriteManager::Vertex),
						(GLbyte *)0 + 4*2 + 4*1 + 4*2
					);
					glEnableVertexAttribArray(Position_vec2);

					GLuint VertexID_uint = glGetAttribLocation(sprite_program, "VertexID");
					glVertexAttribPointer(
						VertexID_uint,
						1,
						GL_UNSIGNED_INT,
						GL_FALSE,
						sizeof(SpriteManager::Vertex),
						(GLbyte *)0 + 4*2 + 4*1 + 4*2 + 4*2
					);
					glEnableVertexAttribArray(VertexID_uint);

					glBindVertexArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					GL_ERRORS();
				}

				GLuint small_to_big_ViewportSize_uvec2   = 0;

				GLuint draw(GLuint old_tex, std::vector<SpriteManager::Vertex> &verts) {

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

					glDrawArrays(GL_TRIANGLE_STRIP, 0, verts.size());

					glBindTexture(GL_TEXTURE_2D, 0);
					glBindVertexArray(0);
					glUseProgram(0);
					glBindFramebuffer(GL_FRAMEBUFFER, 0);

					GL_ERRORS();
					return tex;
				}


			protected:
				// hide old draw function
				GLuint draw(GLuint old_tex) {return 0;}

				GLuint sprite_tex;
				GLuint sprite_program;
				GLuint sprite_vao;
				GLuint sprite_vbo;


		};

		GLuint empty_vao = 0;

		GLuint blit_program = 0;
		GLint  blit_program_ViewportOffset_uvec2 = -1;

		GLuint sprite_tex;

		std::unique_ptr<SpriteFramebuffer>     sprite_framebuffer;
		std::unique_ptr<BackgroundFramebuffer> background_framebuffer;

		float total_time = 0.0f;


};
