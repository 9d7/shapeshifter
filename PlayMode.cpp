#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "Renderer.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

Load < Renderer > renderer_(LoadTagDefault, []() -> Renderer const * {
	return new Renderer();
});

PlayMode::PlayMode() : renderer(*renderer_) {

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	return false;
}

void PlayMode::update(float elapsed) {
	renderer.update(elapsed);
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	renderer.draw(drawable_size);
	GL_ERRORS();
}
