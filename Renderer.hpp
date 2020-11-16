#pragma once

#include <glm/glm.hpp>
#include "GL.hpp"

// Notes about the renderer:
// 1) It operates in pixel space: The full screen is 320x240 pixels.

class Renderer {

public:

	// basic functions
	Renderer();
	~Renderer();

	void update(float elapsed);
	void draw(const glm::uvec2 &drawable_size);

	enum : size_t {
		ScreenWidth = 320,
		ScreenHeight = 240
	};

	// character controls

	// position: [(0, 0), (320, 240)], rotation: [0, 2*pi]
	void update_char_position(const glm::vec2 &position, float rotation);
	void update_char_thrusters(bool thrust);

private:

	float total_elapsed = 0.0f;

	// Initially, everything renders to tiny
	GLuint tiny_fbo;
	GLuint tiny_tex;
	GLuint tiny_rbo; // for depth

	// for framebuffer renders--simple quad that fills the screen.
	// vertices here only store texture coordinates and position, both
	// as vec2s. This is used for the texture_program program.
	GLuint quad_vbo;
	GLuint quad_vao;


};
