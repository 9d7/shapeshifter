#pragma once

#include <variant>
#include "Numeric.hpp"
#include <regex>
#include <string>
#include <vector>
#include <EnemyManager.hpp>

class Level {
	public:
		
		typedef std::unordered_map<std::string, std::variant<ValueType, std::vector<ValueType>>> WaveMap;
		typedef std::vector<WaveMap> WaveVector;

		typedef std::unordered_map<std::string, std::variant<ValueType, WaveVector>> RoomMap;
		typedef std::vector<RoomMap> RoomVector;

		typedef std::unordered_map<std::string, std::variant<ValueType, RoomVector>> LevelMap;
		typedef std::unordered_map<std::string, LevelData> LevelsData;

		LevelsData levels_map;

		static const std::string& str(const std::string& level_name, const std::string& key);
		static Numeric& num(const std::string& level_name, const std::string& key);

		// Generates enemy wave and returns wave time and kill limits. <time, kills, boss health>
		std::vector<float> generate_wave(const std::string& level_name, const int room_number, const int wave_number, EnemyManager& enemies);

	protected:
		//std::unordered_map<std::string, Numeric::ValueStore> level_map;

		void load_levels();
	

}