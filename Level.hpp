#pragma once

#include <variant>
#include "Numeric.hpp"
#include <regex>
#include <string>
#include <vector>
#include "EnemyManager.hpp"
#include "BulletManager.hpp"
#include <unordered_map>
#include <iostream>
#include <yaml-cpp/yaml.h>

class Level {
public:
	Level(std::shared_ptr<EnemyManager> enemy_mgr);
	~Level() = default;

	std::vector<std::string> level_names { // Add levels here in their order
		"level one",
		"level two"
	};

	enum LevelStatus {
		NoAdvance,
		NextWave,
		NextRoom,
		NextLevel
	};

	int get_current_level();
	int get_current_room();
	int get_current_wave();
	int get_number_of_rooms();
	int get_number_of_waves();

	glm::vec2 get_spawn_point(int level);

	YAML::Node get_level_info(int level, const std::string& key);
	YAML::Node get_room_info(int level, int room, const std::string& key);
	YAML::Node get_wave_info(int level, int room, int wave, const std::string& key);
	YAML::Node get_wave_info();

	Bullet::Color get_random_color();
	Bullet::Color get_enemy_color(const YAML::Node& in);
	glm::vec2 get_enemy_spawn_position(const YAML::Node& in);
	int get_number_of_enemies(const YAML::Node& in);
	glm::vec2 get_room_bounds(int room);
	glm::vec2 get_room_center(int room);

	void spawn_wave();
	int start_next_wave(); // increments current_wave, calls spawn, returns wave index. Returns -1 if no more waves
	int start_next_room();
	int start_level(int level_number); // set initial values for a level
	LevelStatus advance(); // called when you finish a wave/room/level, i.e. current area is done

	LevelStatus update(float elapsed);
	void update_limits(float elapsed);
	
	bool is_limits_hit();
	void set_limits();

	uint8_t hardcode_menu = 0;

protected:

	int current_level = -1;
	int current_room = -1;
	int current_wave = -1;
	YAML::Node levels_yaml;
	
	std::shared_ptr<EnemyManager> enemies;

	// Limits. If any limit is met, the next wave (not room!) advances
	float time_limit = -1.0f;   // -1 == No time limit, optional
	int kill_limit = -1;        // -1 == Limit is kill all enemies
	int boss_health_limit = -1; // -1 == Limit is kill the boss, optional

	float time_left = 0.0f;
	int kills_left = 0;
	int boss_health_left = 0;
	int total_enemies = 0;;

	


};