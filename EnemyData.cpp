#include "EnemyData.hpp"
#include "data_path.hpp"
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <yaml-cpp/yaml.h>
#include <regex>
#include <random>

namespace {

	typedef std::variant<std::string, EnemyData::Numeric> ValueType;
	typedef std::unordered_map<std::string, ValueType>    ValueStore;
	typedef std::unordered_map<std::string, ValueStore>   EnemyMap;

	EnemyMap enemies;

	typedef std::pair<ValueStore, std::vector<ValueStore>> Attack;
	typedef std::map<float, Attack> AttackList;
	std::unordered_map<std::string, AttackList> attacks;

	auto rng = std::bind(std::uniform_real_distribution<float>(0.0f, 1.0f), std::mt19937());

	void load_enemies() {

		YAML::Node yaml = YAML::LoadFile(data_path("enemies.yaml"));

		auto parse_arg = [](const std::string &in) -> ValueType {

			// first--remove all spaces from string
			// (-?\d+\.?\d*)\s*\.\.\s*(-?\d+\.?\d*) = 2.5..3.5
			// (?:(?:-?\d+\.?\d*)\s*,\s*)+(?:-?\d+\.?\d*) = comma separated list, no captures
			// (-?\d+\.?\d*) = float
			static std::regex range_regex (R"(^\s*(-?\d*\.?\d+)\s*\.\.\s*(-?\d*\.?\d+)\s*$)");
			static std::regex choice_regex (R"(^\s*((-?\d*\.?\d+)\s*,\s*)+(-?\d*\.?\d+)\s*$)");
			static std::regex full_float_regex (R"(^\s*(-?\d*\.?\d+)\s*$)");
			static std::regex float_regex (R"(-?\d*\.?\d+)");

			std::smatch sm;
			if (std::regex_match(in, sm, range_regex)) { // range

				printf("Matched '%s' as range\n", in.c_str());
				return EnemyData::Numeric(EnemyData::Numeric::Range(
					std::atof(sm[1].str().c_str()),
					std::atof(sm[2].str().c_str())
				));
			}

			if (std::regex_match(in, choice_regex)) { // choice

				printf("Matched '%s' as choice\n", in.c_str());
				std::string i = in;
				std::regex_iterator<std::string::iterator> rit ( i.begin(), i.end(), float_regex );
				std::regex_iterator<std::string::iterator> rend;

				EnemyData::Numeric::Choice ret;
				while (rit != rend) {
					ret.push_back(std::atof(rit->str().c_str()));
					rit++;
				}
				return EnemyData::Numeric(ret);
			}

			if (std::regex_match(in, sm, full_float_regex)) { // float
				printf("Matched '%s' as float\n", in.c_str());
				return EnemyData::Numeric(std::atof(sm[1].str().c_str()));
			}

			// none matched -- assume a string
			printf("Matched '%s' as string\n", in.c_str());
			return in;
		};

		ValueStore default_attack;
		ValueStore default_bullet;
		ValueStore default_enemy;
		float      default_attack_chance = 0.0f;

		YAML::Node default_ = yaml["default"];
		for (auto it = default_["enemy"].begin(); it != default_["enemy"].end(); it++) {
			default_enemy[it->first.as<std::string>()] =
				parse_arg(it->second.as<std::string>());
		}
		enemies["default"] = default_enemy;

		for (auto it = default_["attack"].begin(); it != default_["attack"].end(); it++) {
			std::string name = it->first.as<std::string>();
			if (name == "chance") {
				default_attack_chance = it->second.as<float>();
			} else {
				default_attack[name] = parse_arg(it->second.as<std::string>());
			}
		}

		for (auto it = default_["bullet"].begin(); it != default_["bullet"].end(); it++) {
			default_bullet[it->first.as<std::string>()] =
				parse_arg(it->second.as<std::string>());
		}

		for (auto it = yaml.begin(); it != yaml.end(); it++) {

			std::string name = it->first.as<std::string>();
			YAML::Node value = it->second;
			if (name == "default") continue;

			ValueStore new_;
			for (auto it2 = value.begin(); it2 != value.end(); it2++) {

				std::string key = it2->first.as<std::string>();

				if (key == "attacks") {

					YAML::Node attacks_node = it2->second;
					AttackList attack_list;
					float total = 0.0f;

					for (auto it3 = attacks_node.begin(); it3 != attacks_node.end(); it3++) {

						Attack attack;
						bool found_chance = false;
						for (auto it4 = it3->begin(); it4 != it3->end(); it4++) {

							std::string attack_param = it4->first.as<std::string>();
							if (attack_param == "bullets") {

								for (auto it5 = it4->second.begin(); it5 != it4->second.end(); it5++) {
									ValueStore bullet;
									for (auto it6 = it5->begin(); it6 != it5->end(); it6++) {
										bullet[it6->first.as<std::string>()] =
											parse_arg(it6->second.as<std::string>());
									}

									// put in defaults if they don't exist
									for (auto db : default_bullet) {
										bullet.emplace(db);
									}

									attack.second.emplace_back(bullet);
								}

							} else if (attack_param == "chance") {

								total += it4->second.as<float>();
								found_chance = true;
							} else {

								attack.first[attack_param] = parse_arg(it4->second.as<std::string>());
							}

						}

						for (auto da : default_attack) {
							attack.first.emplace(da);
						}

						if (!found_chance) {
							total += default_attack_chance;
						}
						attack_list[total] = attack;

					}

					attacks[name] = attack_list;

				} else {
					new_[key] = parse_arg(it2->second.as<std::string>());
				}

			}

			for (auto de : default_enemy) {
				new_.emplace(de);
			}
			enemies[name] = new_;

		}

	}
}

std::mt19937 EnemyData::Numeric::mt (std::chrono::system_clock::now().time_since_epoch().count());
std::uniform_real_distribution<float> EnemyData::Numeric::dist (0.0f, 1.0f);

EnemyData::Numeric &EnemyData::num(const std::string &enemy_name, const std::string &key) {

	if (enemies.size() == 0) { load_enemies(); }

	return std::get<Numeric>(enemies[enemy_name][key]);

}

const std::string &EnemyData::str(const std::string &enemy_name, const std::string &key) {

	if (enemies.size() == 0) { load_enemies(); }

	return std::get<std::string>(enemies[enemy_name][key]);
}
