#include "Mode.hpp"

#include "Renderer.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <unordered_map>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	glm::vec2 char_velocity = glm::vec2(0, 0);
	glm::vec2 char_position = glm::vec2(0, 0);
	glm::vec2 camera_position = glm::vec2(0, 0);
	glm::vec2 camera_vector_from_player = glm::vec2(0, 0);

	Renderer renderer;

	// Tracking inputs
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	};

	std::unordered_map<SDL_KeyCode, Button> buttons{
		{SDLK_w, Button()},
		{SDLK_a, Button()},
		{SDLK_s, Button()},
		{SDLK_d, Button()},
		{SDLK_r, Button()}
	};

	
};
