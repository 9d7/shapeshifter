#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>

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

	//----- game state -----
	reactphysics3d::PhysicsCommon physicsCommon;
	reactphysics3d::PhysicsWorld* world;
	reactphysics3d::RigidBody* bodyPlayer;
	reactphysics3d::RigidBody* bodyEnemy;
	reactphysics3d::RigidBody* bodyFloor;
	reactphysics3d::Collider* collider1; 
	reactphysics3d::Collider* collider2; 
	reactphysics3d::Collider* collider3; 

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	};

	std::unordered_map<SDL_KeyCode, Button> buttons{
		{SDLK_w, Button()},
		{SDLK_a, Button()},
		{SDLK_s, Button()},
		{SDLK_d, Button()},
		{SDLK_q, Button()},
		{SDLK_e, Button()},
		{SDLK_r, Button()},
		{SDLK_t, Button()},
		{SDLK_o, Button()},
		{SDLK_SPACE, Button()}
	};

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;
	Scene::Transform *player = nullptr;
	Scene::Transform *shifted = nullptr;
	Scene::Transform *sphere = nullptr;
	Scene::Transform *plane = nullptr;
	Scene::Transform *playerSphere = nullptr;

	bool is_human = true;
	float transformDelay = 0.0f;
	float transformTimer = 0.0f;

	glm::vec3 get_leg_tip_position();

	glm::vec2 move_character(bool up, bool down, bool left, bool right);

	//music coming from the tip of the leg (as a demonstration):
	std::shared_ptr< Sound::PlayingSample > leg_tip_loop;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
