#include "Mode.hpp"

#include "Renderer.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <unordered_map>



struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	// Constants
	static constexpr float MOVE_FORCE = 600.0f; // move_force is in pixels/s^2 (it's actually acceleration, don't tell anybody)
	static constexpr float MAX_VELOCITY = 400.0f; // max_velocity is in pixels/s
	static constexpr float FRICTION = 0.95f;
	static constexpr glm::vec2 PIXEL_SCREEN_CENTER = glm::vec2(160, 120); // The center of the pixel screen

	struct bullet_wrapper {
		Renderer::Bullet b;
		glm::vec2 direction;
		float speed;
	};

	struct enemy_wrapper {
		Renderer::Enemy e;
		float bullet_timer;
	};

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	// Additional functions
	void update_force_vector(glm::vec2 &force_vector);
	//void reset_downs();
	void dev_mode_update();
	void shoot_bullet();

	void check_collisions();
	void update_bullets(std::deque<bullet_wrapper> &bullets, float elapsed);
	void shoot_enemy_bullet(Renderer::Enemy &e, float speed);

	void move_soldier(enemy_wrapper &ewrap, float elapsed);
	void move_hunter(enemy_wrapper &ewrap, float elapsed);
	void generate_enemy();


	// Tracking positions and velocities
	glm::vec2 force_vector = glm::vec2(0, 0);
	glm::vec2 player_velocity = glm::vec2(0, 0);
	glm::vec2 player_position = glm::vec2(0, 0);
	glm::vec2 camera_position = glm::vec2(0, 0);
	glm::vec2 mouse_position = glm::vec2(0, 0);
	glm::vec2 mouse_vector_from_player = glm::vec2(0, 0);

	Renderer renderer;

	std::deque<bullet_wrapper> enemy_bullets, player_bullets;
    std::deque<enemy_wrapper> enemies;

 
    float max_bullet_time = 5.0f;
  	Renderer::BulletColor player_color = Renderer::Red; 

	bool dead = false;
	int score = 0;
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
		{SDLK_r, Button()},
		{SDLK_j, Button()},
		{SDLK_k, Button()},
		{SDLK_v, Button()},
		{SDLK_m, Button()},
		{SDLK_p, Button()},
		{SDLK_l, Button()},
		{SDLK_h, Button()},
		{SDLK_o, Button()},

	};

	// Dev mode
	bool dev_mode = true;
	float dev_velocity_multiplier = 1.0f;
	bool dev_velocity_rotation_mode = false;
	bool dev_position_debug = false;
	bool dev_show_dev_values = true;


};
a