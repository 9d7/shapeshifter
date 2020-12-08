#include "Level.hpp"
#include "data_path.hpp"
#include <map>
#include <yaml-cpp/yaml.h>

const std::string& Level::str(const std::string& level_name, const int room, const int wave, const std::string& key) {
	
	if (levels_map.size() == 0) { load_levels(); }

	return std::get<std::string>(levels_map[level_name][room][wave][key]);
}

Numeric& Level::num(const std::string& level_name, const int room, const int wave, const std::string& key)
{
	if (levels_map.size() == 0) { load_levels(); }

	return std::get<Numeric>(levels_map[level_name][room][wave][key]);
}

// TODO fix this
std::vector<WaveEnemy> Level::get_wave_enemies(const std::string& level_name, const int room, const int wave) {
	std::vector<WaveEnemy> enemies;

	for (std::string e : EnemyList) {
		if (levels_map[level_name][room][wave].find(e) != levels_map[level_name][room][wave].end()) {
			WaveEnemy new_e;
			new_e.name = e;
			new_e.number = int(levels_map[level_name][room][wave][e][0]);
			new_e.color = Bullet::Blue; //levels_map[level_name][room][wave][e][1];
			for (int i = new_e.number) {
				new_e.locations.emplace_back(glm::vec2(0.0f));
			}
		}
	}

	return enemies;
}

std::vector<float> Level::generate_wave(const std::string& level_name, const int room, const int wave, std::shared_ptr<EnemyManager> enemies) {
	
	enemies->acquire("hunter", Bullet::Red, glm::vec2(5.0f, 0), Enemy::Hunter);
	std::vector<WaveEnemy> enemy_list = get_wave_enemies(level_name, room, wave);
	for (WaveEnemy e : enemy_list) {
		for (int i = 0; i < e.number; i++) {
			enemies->acquire(e.name, e.color, e.pos);
		}
	}
	return std::vector<float>{0.0f, 0.0f, 0.0f});
}

void Level::load_levels() {

	YAML::Node levels_yaml = YAML::LoadFile(data_path("levels.yaml"));

	Numeric::ValueStore default_level;
	Numeric::ValueStore default_room; // Maybe unneeded?
	Numeric::ValueStore default_wave;

	// Setting up and storing defaults
	YAML::Node default_ = levels_yaml["default"];
	for (auto it = default_["level"].begin(); it != default_["level"].end(); it++) {
		default_level[it->first.as<std::string>()] =
			Numeric::parse(it->second.as<std::string>());
	}
	level_map["default"] = default_level;

	for (auto it = default_["rooms"].begin(); it != default_["rooms"].end(); it++) {
		default_room[it->first.as<std::string>()] =
			Numeric::parse(it->second.as<std::string>());
	}
	for (auto it = default_["waves"].begin(); it != default_["waves"].end(); it++) {
		default_wave[it->first.as<std::string>()] =
			Numeric::parse(it->second.as<std::string>());
	}

	// Parsing through the yaml
	for (auto it = levels_yaml.begin(); it != levels_yaml.end(); it++) { // Each level

		std::string level_name = it->first.as<std::string>();
		YAML::Node level_contents = it->second;
		if (level_name == "default") continue;

		LevelMap current_level;
		
		for (auto it2 = level_contents.begin(); it2 != level_contents.end(); it2++) { // Parsing through one level

			std::string level_key = it2->first.as<std::string>();
			YAML::Node level_value = it2->second;

			if (level_key == "rooms") {

				YAML::Node room_node = level_value;
				
				RoomVector rooms;
				
				for (auto it3 = room_node.begin(); it3 != room_node.end(); it3++) { // Parsing through rooms
					
					YAML::Node room_contents = it3->second;

					RoomMap current_room;
					
					for (auto it4 = room_contents.begin(); it4 != room_contents.end(); it4++) { // Parsing through one room

						std::string room_key = it4->first.as<std::string>(); // you need a key to see a room's contents;)
						YAML::Node room_value = it4->second;
						if (room_key == "waves") {

							YAML::Node wave_node = room_value;

							WaveVector waves;

							for (auto it5 = wave_node.begin(); it5 != wave_node.end(); it5++) { // Parsing through waves
								
								YAML::Node wave_contents = it5->second;

								WaveMap current_wave;

								for (auto it6 = room_contents.begin(); it6 != room_contents.end(); it6++) { // Parsing through one wave
									std::string wave_key = it6->first.as<std::string>();
									YAML::Node wave_value = it6->second;

									// Add wave data to current wave map
									if (wave_value.IsSequence()) { // e.g. an enemy
										std::vector<ValueType> sequence_vector;
										for (auto it7 = wave_value.begin(); it7 != wave_value.end(); it7++) {
											sequence_vector.emplace_back(Numeric::parse((it7->first).as<std::string>()));
										}
										current_wave[wave_key] = sequence_vector;
									}
									else {
										current_wave[wave_key] = Numeric::parse(wave_value.as<std::string>());
									}
								}
								// Add current wave to waves vector
								waves.emplace_back(current_wave);
							}
							// Add waves vector to current room map
							current_room["waves"] = waves;
						}
						else { // Add room data to current room map
							current_room[room_key] = Numeric::parse(room_value.as<std::string>());
						}
					}
					// Add current room map to rooms vector
					rooms.emplace_back(current_room);
				}
				// Add rooms vector to current level map
				current_level["rooms"] = rooms;
			}
			else { // Add level data to current level map
				current_level[level_key] = Numeric::parse(level_value.as<std::string>());
			}
		}
		// and finally, Add the current level map to the levels map!
		levels_map[level_name] = current_level;

	}

	
}