#pragma once

#include <glm/glm.hpp>
#include "GL.hpp"
#include "glm/fwd.hpp"
#include <vector>

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

	// position: [(0, 0), (320, 240)]
	void update_camera_position(const glm::vec2 &position);

private:

	glm::vec2 char_position = glm::vec2(0, 0);
	glm::vec2 camera_position = glm::vec2(0, 0);
	float     char_rotation = 0.0f;

	float total_elapsed = 0.0f;

	// Initially, everything renders to tiny
	GLuint tiny_fbo;
	GLuint tiny_tex;
	GLuint tiny_rbo; // for depth

	// For tiny sprites
	GLuint tiny_vbo;
	GLuint tiny_vao;

	// for framebuffer renders--simple quad that fills the screen.
	// vertices here only store texture coordinates and position, both
	// as vec2s. This is used for the texture_program program.
	GLuint quad_vbo;
	GLuint quad_vao;

	// texture atlas
	GLuint atlas_tex;
	glm::uvec2 atlas_size;

	struct Vertex{
		glm::vec2 position;
		glm::vec2 tex_coord;
		glm::vec4 color;
	};
	static_assert(sizeof(Vertex) == sizeof(float) * 8, "Vertex is not packed");

	std::vector<Vertex> small_verts;


};
