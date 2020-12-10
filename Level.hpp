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

	std::vector<std::string> enemy_names {
		"soldier",
		"ninja",
		"hunter",
		"turret"
	};

	struct WaveEnemy {
		std::string name;
		int number;
		Bullet::Color color;
		std::vector<glm::vec2> locations;
	};

	typedef std::variant<std::string, Numeric> ValueType;
	typedef std::unordered_map<std::string, std::variant<ValueType, std::vector<ValueType>>> WaveMap;
	typedef std::vector<WaveMap> WaveVector;

	typedef std::unordered_map<std::string, std::variant<ValueType, WaveVector>> RoomMap;
	typedef std::vector<RoomMap> RoomVector;

	typedef std::unordered_map<std::string, std::variant<ValueType, RoomVector>> LevelMap;
	typedef std::unordered_map<std::string, LevelMap> LevelsData;

	LevelsData levels_map;

	const std::string& str(const std::string& level_name, const int room, const int wave, const std::string& key);
	Numeric& num(const std::string& level_name, const int room, const int wave, const std::string& key);

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

	void spawn_wave();
	int start_next_wave(); // increments current_wave, calls spawn, returns wave index. Returns -1 if no more waves
	int start_next_room();
	glm::vec2 start_level(int level_number); // set initial values for a level
	bool advance(); // called when you finish a wave/room/level, i.e. current area is done
	bool update(float elapsed);
	void update_limits(float elapsed);
	bool is_limits_hit();
	void set_limits();
	void test(std::shared_ptr<EnemyManager> enemies);

	std::vector<struct WaveEnemy> get_wave_enemies(const std::string& level_name, const int room, const int wave);

	// Generates enemy wave and returns wave time and kill limits. <time, kills, boss health>
	void generate_wave(const std::string& level_name, const int room, const int wave, std::shared_ptr<EnemyManager> enemies);
	void load_levels();
protected:

	int current_level = -1;
	int current_room = -1;
	int current_wave = -1;
	YAML::Node levels_yaml;
	//std::unordered_map<std::string, Numeric::ValueStore> level_map;
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