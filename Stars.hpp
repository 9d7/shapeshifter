#pragma once

#include "GL.hpp"
#include <ctime>
#include <functional>
#include <glm/glm.hpp>
#include <random>
#include <vector>
#include <list>
#include <array>
#include <ctime>

class Stars {

public:

	Stars();
	~Stars();

	#define NUM_INSTANCES 100

	void update(float elapsed);
	void draw(
		const std::array<GLint, NUM_INSTANCES> &TexCoords_ivec2,
		const std::array<GLint, NUM_INSTANCES> &Colors_vec4,
		const std::array<GLint, NUM_INSTANCES> &Positions_ivec2,
		const std::array<GLint, NUM_INSTANCES> &Parallax_float
	);

private:

	// random
	std::mt19937 mt                            = std::mt19937((unsigned int)time(NULL));
	std::uniform_real_distribution<float> real = std::uniform_real_distribution<float>(0.0f, 1.0f);

	// list
	struct Star {
		size_t     animation_index;
		size_t     frame_index;
		float      elapsed;
		// slightly modify speed of different patterns so they
		// don't overlap
		float      speed_modifier;

		glm::ivec2  tex_coords;
		glm::vec4  color;
		glm::ivec2 pos;
		float      parallax;
	};

	std::array<Star, NUM_INSTANCES> stars;

	// animations
	const glm::uvec2 SPRITE_PIP      { 0,  0};
	const glm::uvec2 SPRITE_TWINKLE1 { 8,  0};
	const glm::uvec2 SPRITE_TWINKLE2 {16,  0};
	const glm::uvec2 SPRITE_TWINKLE3 {24,  0};

	const glm::uvec2 SPRITE_EXPAND   {32,  0};
	const glm::uvec2 SPRITE_BURST1   {40,  0};
	const glm::uvec2 SPRITE_BURST2   {48,  0};
	const glm::uvec2 SPRITE_BURST3   {56,  0};

	const glm::uvec2 SPRITE_SHOOT1   { 0,  8};
	const glm::uvec2 SPRITE_SHOOT2   { 8,  8};
	const glm::uvec2 SPRITE_SHOOT3   {16,  8};
	const glm::uvec2 SPRITE_SHOOT4   {24,  8};

	const glm::uvec2 SPRITE_EMPTY    {40,  8};

	const std::vector<std::vector<std::pair<glm::uvec2, float>>> animations {
		{
			{SPRITE_PIP, 120.0f}
		}, {
			{SPRITE_PIP, 5.0f},
			{SPRITE_TWINKLE1, 0.5f},
			{SPRITE_TWINKLE2, 0.5f},
			{SPRITE_TWINKLE3, 0.5f},
			{SPRITE_TWINKLE2, 0.5f},
			{SPRITE_TWINKLE1, 0.5f},
		}, {
			{SPRITE_PIP, 0.5f},
			{SPRITE_TWINKLE1, 0.5f},
			{SPRITE_EXPAND, 0.5f},
			{SPRITE_TWINKLE1, 0.5f}
		}, {
			{SPRITE_EMPTY, 10.0f},
			{SPRITE_PIP, 0.25f},
			{SPRITE_TWINKLE1, 0.25f},
			{SPRITE_BURST1, 0.25f},
			{SPRITE_BURST2, 0.25f},
			{SPRITE_BURST3, 0.25f}
		}, {
			{SPRITE_EMPTY, 30.0f},
			{SPRITE_SHOOT1, 0.10f},
			{SPRITE_SHOOT2, 0.10f},
			{SPRITE_SHOOT3, 0.10f},
			{SPRITE_SHOOT4, 0.10f}
		}
	};
};
