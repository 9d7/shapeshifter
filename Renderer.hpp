#pragma once

#include <glm/glm.hpp>
#include "GL.hpp"
#include "Stars.hpp"
#include "glm/fwd.hpp"
#include <iterator>
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

	struct Vertex{
		glm::vec2 position;
		glm::vec2 tex_coord;
		glm::vec4 color;
	};
	static_assert(sizeof(Vertex) == sizeof(float) * 8, "Vertex is not packed");

	// character controls

	// position: [(0, 0), (320, 240)], rotation: [0, 2*pi]
	void update_char_position(const glm::vec2 &position, float rotation);
	void update_char_thrusters(bool thrust);

	// position: [(0, 0), (320, 240)]
	void update_camera_position(const glm::vec2 &position);

	// bullets
	enum BulletColor {
		Red,
		Blue,
		Purple
	};

	struct Bullet_ {
		glm::vec2   position;
		BulletColor color;
		float       elapsed;
	};

	typedef std::list<Bullet_>::iterator Bullet;

	Bullet new_bullet(const glm::vec2 &position, BulletColor color);
	void update_bullet_position(Bullet b, const glm::vec2 &position);
	void update_bullet_color(Bullet b, BulletColor color);
	void destroy_bullet(Bullet b);

	enum EnemyType {
		Regular,
		Soldier,
		Turret,
		Shifter
	};

	struct Enemy_ {
		glm::vec2 position;
		float     rotation;
		float     elapsed; // for animation
		float     invuln_time; // for animation
		EnemyType type;
	};
	
	typedef std::list<Enemy_>::iterator Enemy;
	Enemy new_enemy(const glm::vec2 &position, float rotation, EnemyType type);
	void update_enemy_position(Enemy e, const glm::vec2 &position);
	void update_enemy_rotation(Enemy e, float rotation);
	void invuln_enemy(Enemy e);
	void destroy_enemy(Enemy e);


private:

	std::list<Bullet_> bullets;
	std::list<Enemy_> enemies;

	Stars stars;

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

	// for stars
	GLuint star_vbo;
	GLuint star_vao;

	// texture atlas
	GLuint atlas_tex;
	glm::uvec2 atlas_size;

	std::vector<Vertex> small_verts;


};
