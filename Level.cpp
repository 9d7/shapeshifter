#include "Level.hpp"
#include "data_path.hpp"
#include <map>
#include <glm/gtx/rotate_vector.hpp>
#include <string>

//const std::string& Level::str(const std::string& level_name, const int room, const int wave, const std::string& key) {
//	
//	if (levels_map.size() == 0) { load_levels(); }
//
//	return std::get<std::string>((((levels_map[level_name])[room])[wave])[key]);
//}
//
//Numeric& Level::num(const std::string& level_name, const int room, const int wave, const std::string& key)
//{
//	if (levels_map.size() == 0) { load_levels(); }
//
//	return std::get<Numeric>(levels_map[level_name][room][wave][key]);
//}

Level::Level(std::shared_ptr<EnemyManager> enemy_mgr) {

	//std::unordered_map<std::string, std::variant<ValueType, RoomVector>> test = levels_map[""];
	levels_yaml = YAML::LoadFile(data_path("levels.yaml"));
	//printf("yaml: %s\n", levels_yaml.as<std::string>().c_str());
	enemies = enemy_mgr;
}

int Level::start_next_wave() {
	current_wave++;
	
	if (current_wave >= get_number_of_waves()) { // No more waves
		current_wave = -1;
		kills_left = enemies->get_num_enemies(); // all enemies need to die before advancing
	}
	else spawn_wave();

	return current_wave;
}

int Level::start_next_room() {
	current_room++;
	current_wave = -1;
	enemies->enemies.clear();

	if (current_room >= get_number_of_rooms()) current_room = -1;
	else start_next_wave();

	return current_room;
}

glm::vec2 Level::start_level(int level_number) {
	printf("starting %s\n", level_names[level_number].c_str());
	current_level = level_number;
	current_room = -1;
	current_wave = -1;
	return get_spawn_point(current_level);
	// a vector is treated as a yaml node, isSequence, with each data point in that 
}

bool Level::advance() {
	printf("advancing\n");
	printf("wave: %d\n", current_wave);
	// Advances to next wave
	if (start_next_wave() < 0) { 
		// if no more waves, go to next room
		if (start_next_room() < 0) { 
			// if no more rooms in level, go to next level
			// TODO change this so it goes to an in between screen (a shop? just a "level completed, press X to advance"? TBD)
			start_level(current_level); //TODO change this to ++ if you want to advance to the next level, rn just restarts the level
			return true;
		}
	}
	return false;
}

bool Level::update(float elapsed) {
	update_limits(elapsed);
	if (is_limits_hit()) {
		printf("limits hit: %f %d %d || %f %d %d\n", time_left, kills_left, boss_health_left, time_limit, kill_limit, boss_health_limit);
		return advance();
	}
	return false;
}

void Level::update_limits(float elapsed) {
	if (time_left > 0.0f) time_left -= elapsed;
	kills_left -= total_enemies - enemies->get_num_enemies();   // kills == size diff from when we last checked total enemies
	total_enemies -= total_enemies - enemies->get_num_enemies();
	//boss_health_left -= boss_health; // TODO if we want to have different boss stages based on boss health
}

bool Level::is_limits_hit() {
	if (time_limit > 0.0f && time_left <= 0.0f) return true;
	if (kills_left <= 0) return true;
	if (boss_health_limit >= 0 && boss_health_left <= 0) return true;

	return false;
}

void Level::set_limits() {
	total_enemies = enemies->get_num_enemies();
	time_left = time_limit;
	kills_left = kill_limit > 0 ? kill_limit : total_enemies;
	boss_health_left = boss_health_limit;
	if (current_wave == get_number_of_waves()) kills_left = total_enemies;
}

void Level::test(std::shared_ptr<EnemyManager> enemies)
{
	enemies->acquire("turret", Bullet::Blue, glm::vec2(0.0f));
}

/*// TODO fix this
std::vector<struct Level::WaveEnemy> Level::get_wave_enemies(const std::string& level_name, const int room, const int wave) {
	std::vector<struct WaveEnemy> enemies;
	for (std::string e : EnemyList) {
		//std::get<Numeric>(std::get<std::vector<ValueType>>(std::get<WaveVector>(std::get<RoomVector>(levels_map[level_name]["rooms"])[room]["waves"])[wave][e])[0]);


		if ( true ){//std::get<WaveVector>(std::get<RoomVector>(levels_map[level_name]["rooms"])[room]["waves"])[wave] ) {
			struct WaveEnemy new_e;
			new_e.name = e;
			new_e.number = int( std::get<Numeric>(std::get<std::vector<ValueType>>(std::get<WaveVector>(std::get<RoomVector>(levels_map[level_name]["rooms"])[room]["waves"])[wave][e])[0])() );
			new_e.color = Bullet::Blue; //levels_map[level_name][room][wave][e][1];
			for (int i = new_e.number; i > 0; i--) {
				new_e.locations.emplace_back(glm::vec2(0.0f));
			}
			enemies.emplace_back(new_e);
		}
	}

	return enemies;
}

void Level::generate_wave(const std::string& level_name, const int room, const int wave, std::shared_ptr<EnemyManager> enemies) {
	
	//enemies->acquire("hunter", Bullet::Red, glm::vec2(5.0f, 0), Enemy::Hunter);
	std::vector<struct WaveEnemy> enemy_list = get_wave_enemies(level_name, room, wave);
	for (struct WaveEnemy e : enemy_list) {
		for (int i = 0; i < e.number; i++) {
			enemies->acquire(e.name, e.color, e.locations[0]);
		}
	}
	//return std::vector<float>{0.0f, 0.0f, 0.0f});
}

void Level::load_levels() {

	YAML::Node levels_yaml = YAML::LoadFile(data_path("levels.yaml"));

	//Numeric::ValueStore default_level;
	//Numeric::ValueStore default_room; // Maybe unneeded?
	//Numeric::ValueStore default_wave;

	//// Setting up and storing defaults
	//YAML::Node default_ = levels_yaml["default"];
	//for (auto it = default_["level"].begin(); it != default_["level"].end(); it++) {
	//	default_level[it->first.as<std::string>()] =
	//		Numeric::parse(it->second.as<std::string>());
	//}
	//levels_map["default"] = default_level;

	//for (auto it = default_["rooms"].begin(); it != default_["rooms"].end(); it++) {
	//	default_room[it->first.as<std::string>()] =
	//		Numeric::parse(it->second.as<std::string>());
	//}
	//for (auto it = default_["waves"].begin(); it != default_["waves"].end(); it++) {
	//	default_wave[it->first.as<std::string>()] =
	//		Numeric::parse(it->second.as<std::string>());
	//}

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
*/
//TODO get rid of above funcs

int Level::get_current_level() {
	return current_level;
}

int Level::get_number_of_rooms() {
	return int(get_level_info(current_level, "rooms").size());
}

int Level::get_number_of_waves() {
	return int(get_room_info(current_level, current_room, "waves").size());
}

glm::vec2 Level::get_spawn_point(int level) {
	printf("getting spawn point!\n");
	return Numeric::parse_vector(get_level_info(level, "spawn"));
}

YAML::Node Level::get_level_info(int level, const std::string& key) {
	//printf("getting level info\n");
	return levels_yaml[level_names[level]][key];
}

YAML::Node Level::get_room_info(int level, int room, const std::string& key) {
	return get_level_info(level, "rooms")[room][key];
}

YAML::Node Level::get_wave_info(int level, int room, int wave, const std::string& key) {
	return get_room_info(level, room, "waves")[wave][key];
}

YAML::Node Level::get_wave_info() {
	return get_room_info(current_level, current_room, "waves")[current_wave];
}

Bullet::Color Level::get_random_color() {
	float rand_num = (Numeric(Numeric::Range(0.0f,2.0f)))(); //very dumb, don't look eric
	if (rand_num >= 1.0f) return Bullet::Blue;
	else return Bullet::Red;
}

Bullet::Color Level::get_enemy_color(const YAML::Node& in) {
	if (in.size() < 2) return get_random_color(); 
	std::string color_type = Numeric::parse_string(in[1]);
	if (color_type == "red") return Bullet::Blue;
	else if (color_type == "blue") return Bullet::Red;
	else return get_random_color(); //TODO implement split
}

glm::vec2 Level::get_enemy_spawn_position(const YAML::Node& in) {
	// TODO @jinwoo, if you want to edit this to make spawns more robust, go ahead
	// I just have a lot more things to implement and this is good enough for now
	// To make more robust, would have to edit the yaml. Easiest way to avoid that is just edit the default
	float theta = (Numeric(Numeric::Range(0.0f, 6.28f)))();
	glm::vec2 spawn_vec = glm::vec2(1.0f, 0.0f);
	if (in.size() < 3) {
		float radius = (Numeric(Numeric::Range(5.0f, 10.0f)))();
		glm::vec2 spawn_pos = glm::rotate(spawn_vec, theta) * radius;
		return spawn_pos; // default value
	}
	float radius = Numeric::parse_num(in[2])();
	glm::vec2 spawn_pos = glm::rotate(spawn_vec, theta) * radius;
	return spawn_pos;
}

int Level::get_number_of_enemies(const YAML::Node& in) {
	if (in.size() < 1)	return 1; // default value TODO make this a class variable so you can change it easily
	else return int(Numeric::parse_num(in[0])()); 
}

void Level::spawn_wave() {
	printf("spawning level %d room %d wave %d\n", current_level, current_room, current_wave);
	YAML::Node wave = get_wave_info();
	time_limit = -1.0f;
	kill_limit = -1;
	boss_health_limit = -1;
	for (auto it = wave.begin(); it != wave.end(); it++) {
		std::string key = it->first.as<std::string>();
		if (key == "time limit") time_limit = Numeric::parse_num(it->second)();
		else if (key == "kill limit") kill_limit = int(Numeric::parse_num(it->second)());
		else if (key == "health limit") boss_health_limit = int(Numeric::parse_num(it->second)());
		else { // key is an enemy name
			int number_of_enemies = get_number_of_enemies(it->second);
			for (int i = 0; i < number_of_enemies; i++) {
				enemies->acquire(key, get_enemy_color(it->second), get_enemy_spawn_position(it->second));
			}
		}
	}
	set_limits();
	//std::vector<int> limits{ time_limit, kill_limit, boss_health_limit };
}
