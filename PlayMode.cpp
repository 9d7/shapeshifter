#include "PlayMode.hpp"
#include "Load.hpp"
#include <memory>

/*
 * A note to anybody who's developing on PlayMode:
 * This is probably pretty close to how PlayMode.cpp will end up in our
 * final. I'd suggest not touching this unless you're really sure about
 * it, since any changes that *need* to be made in PlayMode.cpp are probably
 * a good sign that you're breaking MVC rules.
 *
 * Controller contains a pointer to Model, which contains a pointer to View--
 * you should probably handle whatever you need to do in one of those classes.
 *
 * - Eric
 */

PlayMode::PlayMode() {

	// using shared ptrs to handle deallocation
	view = std::make_shared<View>();
	model = std::make_shared<Model>(std::weak_ptr<View>(view));
	controller = std::make_shared<Controller>(std::weak_ptr<Model>(model));

}

PlayMode::~PlayMode() {}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	return controller->handle_event(evt, window_size);
}

void PlayMode::update(float elapsed) {

	controller->update(elapsed);
	model->update(elapsed);
	view->update(elapsed);

}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	view->draw(drawable_size);
}
