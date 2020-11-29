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

		// use this to get new sprites, as well as Animation::find_* functions
		SpriteManager sprites {};

		void update_camera(const glm::vec2 &pos);

	private:

		glm::vec2 camera_position {0.0f, 0.0f};

		GLuint empty_vao = 0;

		GLuint blit_program = 0;
		GLint  blit_program_ViewportOffset_uvec2 = -1;

		GLuint sprite_tex;

		std::unique_ptr<SpriteFramebuffer>     sprite_framebuffer;
		std::unique_ptr<BackgroundFramebuffer> background_framebuffer;

		float total_time = 0.0f;


};
