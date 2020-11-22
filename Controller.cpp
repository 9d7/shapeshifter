#include "Controller.hpp"
#include "Model.hpp"
#include <memory>

Controller::Controller(std::weak_ptr<Model> model_) : model(model_) {}

void Controller::update(float elapsed) {}

bool Controller::handle_event(
	const SDL_Event &evt,
	const glm::uvec2 &window_size
) {
	return false;
}
