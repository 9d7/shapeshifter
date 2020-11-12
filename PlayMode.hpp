#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>

#include <vector>
#include <deque>
#include <unordered_map>

struct level_map {
	std::unordered_map<rp3d::Collider*, rp3d::RigidBody*> walkables;
	std::unordered_map<rp3d::Collider*, rp3d::RigidBody*> triggers;
	rp3d::RigidBody* flagrb;
	rp3d::Collider* flagcol; 
};

// Your event listener class 
class YourEventListener : public rp3d::EventListener {
	public: 
		std::vector<rp3d::Collider*> trigs;
		std::vector<rp3d::Collider*> cols;

	virtual void onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) override {
		trigs.clear();
    	for (unsigned int p = 0; p < callbackData.getNbOverlappingPairs(); p++) { 
			// Get the contact pair 
			rp3d::OverlapCallback::OverlapPair contactPair = callbackData.getOverlappingPair(p); 
			rp3d::Collider *c1 = contactPair.getCollider1();
			rp3d::Collider *c2 = contactPair.getCollider2();
			trigs.push_back(c1);
			trigs.push_back(c2);
    	} 
  	}

	virtual void onContact(const CollisionCallback::CallbackData& callbackData) override { 
		cols.clear();
		for (unsigned int p = 0; p < callbackData.getNbContactPairs(); p++) { 
		// Get the contact pair 
			CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p); 
			rp3d::Collider *r1 = contactPair.getCollider1();
			rp3d::Collider *r2 = contactPair.getCollider2();
			cols.push_back(r1);
			cols.push_back(r2);
    	}
	}
};

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----
	void level_1();
	void create_rigid_box(reactphysics3d::Vector3 pos, reactphysics3d::Vector3 size, bool trigger);
	void handle_triggers();
	void handle_dynamic_collisions();

	reactphysics3d::PhysicsCommon physicsCommon;
	reactphysics3d::PhysicsWorld* world;
	reactphysics3d::RigidBody* body_player;
	reactphysics3d::RigidBody* body_enemy;
	reactphysics3d::Collider* collider1; 
	reactphysics3d::Collider* collider2; 

	YourEventListener listener;
	level_map current_map;

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
		{SDLK_k, Button()},
		{SDLK_SPACE, Button()}
	};

	std::string controls_text[2] = {
		"WASD moves your character; SPACE to shapeshift; K to reset",
		"QERT moves your character; SPACE to shapeshift; K to reset"
	};

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;
	Scene::Transform *player = nullptr;
	Scene::Transform *shifted = nullptr;
	Scene::Transform *sphere = nullptr;
	Scene::Transform *plane = nullptr;
	Scene::Transform *playerSphere = nullptr;

	bool is_human = true;
	bool is_dead = false;
	bool level_cleared = false;
	float transformDelay = 0.0f;
	float transformTimer = 0.0f;


	reactphysics3d::Vector3 move_character(bool up, bool down, bool left, bool right);

	//music coming from the tip of the leg (as a demonstration):
	std::shared_ptr< Sound::PlayingSample > leg_tip_loop;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
