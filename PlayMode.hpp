#include "Mode.hpp"

#include "Model.hpp"
#include "View.hpp"
#include "Controller.hpp"
#include <memory>

struct PlayMode : Mode {

	PlayMode(std::shared_ptr<Mode> menu_);
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	// MVC
	std::shared_ptr<Model>      model      = nullptr;
	std::shared_ptr<View>       view       = nullptr;
	std::shared_ptr<Controller> controller = nullptr;

	std::shared_ptr<Mode>   menu;

};
