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


class Level {
public:
	Level();
	~Level() = default;


	std::vector<std::string> EnemyList{
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

	std::vector<struct WaveEnemy> get_wave_enemies(const std::string& level_name, const int room, const int wave);

	// Generates enemy wave and returns wave time and kill limits. <time, kills, boss health>
	void generate_wave(const std::string& level_name, const int room, const int wave, std::shared_ptr<EnemyManager> enemies);
	void load_levels();
protected:
	//std::unordered_map<std::string, Numeric::ValueStore> level_map;


	


};