#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "Prism.hpp"

#include <glm/glm.hpp>
#include <reactphysics3d/engine/PhysicsCommon.h>
#include <reactphysics3d/engine/PhysicsWorld.h>
#include <reactphysics3d/reactphysics3d.h>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----
	reactphysics3d::PhysicsCommon common;
	reactphysics3d::PhysicsWorld *world = nullptr;
	std::vector<Prism> prisms;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, space;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;
	Scene::Transform *player = nullptr;
	Scene::Transform *shifted = nullptr;
	Scene::Transform *sphere = nullptr;
	Scene::Transform *plane = nullptr;
	Scene::Transform *playerSphere = nullptr;

	bool isHuman = true;
	float transformDelay = 0.0f;
	float transformTimer = 0.0f;

	glm::vec3 get_leg_tip_position();

	//music coming from the tip of the leg (as a demonstration):
	std::shared_ptr< Sound::PlayingSample > leg_tip_loop;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
