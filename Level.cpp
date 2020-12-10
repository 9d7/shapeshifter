#include "Level.hpp"
#include "data_path.hpp"
#include <map>
#include <glm/gtx/rotate_vector.hpp>
#include <string>

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

int Level::start_level(int level_number) {
	printf("starting %s\n", level_names[level_number].c_str());
	current_level = level_number;
	current_room = -1;
	current_wave = -1;
	if (current_level >= level_names.size()) current_level = 0;
	start_next_room();
	return current_level;

	// return get_spawn_point(current_level);
	// a vector is treated as a yaml node, isSequence, with each data point in that 
}

Level::LevelStatus Level::advance() {
	printf("advancing\n");
	// Advances to next wave
	if (start_next_wave() < 0) { 
		// if no more waves, go to next room
		if (start_next_room() < 0) { 
			// if no more rooms in level, go to next level
			// TODO change this so it goes to an in between screen (a shop? just a "level completed, press X to advance"? TBD)
			start_level(current_level); //TODO change this to ++ if you want to advance to the next level, rn just restarts the level
			return NextLevel;
		}
		return NextRoom;
	}
	return NextWave;
}

Level::LevelStatus Level::update(float elapsed) {
	update_limits(elapsed);
	if (is_limits_hit()) {
		printf("limits hit: %f %d %d || %f %d %d\n", time_left, kills_left, boss_health_left, time_limit, kill_limit, boss_health_limit);
		return advance();
	}
	return NoAdvance;
}

void Level::update_limits(float elapsed) {
	if (time_left > 0.0f) time_left -= elapsed;
	kills_left -= total_enemies - enemies->get_num_enemies();    // kills == size diff from when we last checked total enemies
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

int Level::get_current_level() {
	return current_level;
}

int Level::get_current_room() {
	return current_room;
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
	if (color_type == "blue") return Bullet::Blue;
	else if (color_type == "red") return Bullet::Red;
	else return get_random_color(); //TODO implement split
}

glm::vec2 Level::get_enemy_spawn_position(const YAML::Node& in) {
	float theta = (Numeric(Numeric::Range(0.0f, 6.28f)))();
	float radius = (Numeric(Numeric::Range(150.0f, 250.0f)))();
	glm::vec2 spawn_vec = glm::vec2(1.0f, 0.0f);
	glm::vec2 spawn_pos = glm::vec2(0.0f, 0.0f);
	
	if (in.size() <= 2) { // default value, no spawn type provided
		spawn_pos = glm::rotate(spawn_vec, theta) * radius;
		return spawn_pos; 
	}

	// spawn type provided
	std::string spawn_type = Numeric::parse_string(in[2]);
	if (spawn_type == "point") { // point requires xpos and ypos to be provided 
		float x_diff = 0.0f;
		float y_diff = 0.0f;
		int num_enemies = get_number_of_enemies(in);
		if (num_enemies > 1) {
			float displacement = 8.0f * num_enemies; // TODO make this number adjustable
			x_diff = (Numeric(Numeric::Range(-displacement, displacement)))();
			y_diff = (Numeric(Numeric::Range(-displacement, displacement)))();
		}
		spawn_pos = glm::vec2( Numeric::parse_num(in[3])() + x_diff, Numeric::parse_num(in[4])() + y_diff );
	}
	else if (in.size() <= 5) { // radius and theta provided
		radius = Numeric::parse_num(in[3])();
		theta = Numeric::parse_num(in[4])();
		spawn_pos = glm::rotate(spawn_vec, theta) * radius;
	}
	else if (in.size() <= 4) { // only radius provided
		radius = Numeric::parse_num(in[3])();
		spawn_pos = glm::rotate(spawn_vec, theta) * radius;
	}
	else { // only spawn type provided (but why do this?)
		spawn_pos = glm::rotate(spawn_vec, theta) * radius;
	}
	return spawn_pos;
}

int Level::get_number_of_enemies(const YAML::Node& in) {
	if (in.size() < 1)	return 1; // default value TODO make this a class variable so you can change it easily
	else return int(Numeric::parse_num(in[0])()); 
}

glm::vec2 Level::get_room_bounds(int room) {
	return Numeric::parse_vector(get_room_info(current_level, room, "bounds"));
}

glm::vec2 Level::get_room_center(int room) {
	return Numeric::parse_vector(get_room_info(current_level, room, "center"));
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
}
