#pragma once

#include <glm/glm.hpp>
#include "SpriteFramebuffer.hpp"
#include "BackgroundFramebuffer.hpp"
#include "GL.hpp"
#include "SpriteManager.hpp"
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


		GLuint empty_vao = 0;

		GLuint blit_program = 0;
		GLint  blit_program_ViewportOffset_uvec2 = -1;

		GLuint sprite_tex;

		std::unique_ptr<SpriteFramebuffer>     sprite_framebuffer;
		std::unique_ptr<BackgroundFramebuffer> background_framebuffer;

		float total_time = 0.0f;


};
