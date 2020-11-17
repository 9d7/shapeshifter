#include "Mode.hpp"

#include "Renderer.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

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

	Renderer renderer;

};
