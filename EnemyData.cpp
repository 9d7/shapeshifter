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
#include "Numeric.hpp"

namespace {

	typedef std::variant<std::string, Numeric> ValueType;
	typedef std::unordered_map<std::string, ValueType>    ValueStore;
	typedef std::unordered_map<std::string, ValueStore>   EnemyMap;

	EnemyMap enemies;

	typedef std::vector<ValueStore> Attack;
	typedef std::map<float, BulletSequencer> AttackList;
	std::unordered_map<std::string, AttackList> attacks;

	void load_enemies() {

		YAML::Node yaml = YAML::LoadFile(data_path("enemies.yaml"));

		auto parse_arg = [](const std::string &in) -> ValueType {

			// wow, regex is so pretty and readable. :)
			static std::regex range_regex (R"(^\s*(-?\d*\.?\d+)\s*\.\.\s*(-?\d*\.?\d+)\s*$)");
			static std::regex choice_regex (R"(^\s*((-?\d*\.?\d+)\s*,\s*)+(-?\d*\.?\d+)\s*$)");
			static std::regex full_float_regex (R"(^\s*(-?\d*\.?\d+)\s*$)");
			static std::regex float_regex (R"(-?\d*\.?\d+)");

			std::smatch sm;
			if (std::regex_match(in, sm, range_regex)) { // range

				return Numeric(Numeric::Range(
					(float)std::atof(sm[1].str().c_str()),
					(float)std::atof(sm[2].str().c_str())
				));
			}

			if (std::regex_match(in, choice_regex)) { // choice

				std::string i = in;
				std::regex_iterator<std::string::iterator> rit ( i.begin(), i.end(), float_regex );
				std::regex_iterator<std::string::iterator> rend;

				Numeric::Choice ret;
				while (rit != rend) {
					ret.push_back((float)std::atof(rit->str().c_str()));
					rit++;
				}
				return Numeric(ret);
			}

			if (std::regex_match(in, sm, full_float_regex)) { // float
				return Numeric((float)std::atof(sm[1].str().c_str()));
			}

			// none matched -- assume a string
			return in;
		};

		ValueStore default_bullet;
		ValueStore default_enemy;
		float      default_attack_chance = yaml["default"]["chance"].as<float>();

		YAML::Node default_ = yaml["default"];
		for (auto it = default_["enemy"].begin(); it != default_["enemy"].end(); it++) {
			default_enemy[it->first.as<std::string>()] =
				parse_arg(it->second.as<std::string>());
		}
		enemies["default"] = default_enemy;

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

									attack.emplace_back(bullet);
								}

							} else if (attack_param == "chance") {

								total += it4->second.as<float>();
								found_chance = true;

							} else {
								std::runtime_error("unexpected attack thing");
							}

						}

						if (!found_chance) {
							total += default_attack_chance;
						}

						std::list<BulletSequencer::AbstractBulletInfo> bs_bullets;

						for (auto a : attack) {
							bs_bullets.push_back(BulletSequencer::AbstractBulletInfo {
								std::get<Numeric>(a["time"]),
								std::get<Numeric>(a["velocity"]),
								std::get<Numeric>(a["angle"]),
								std::get<Numeric>(a["color"]),
								std::get<Numeric>(a["number"]),
								std::get<std::string>(a["sequence"]) == "parallel",
								std::get<std::string>(a["dispatch"]) == "with",
							});
						}

						attack_list.emplace(total, BulletSequencer(bs_bullets));

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


Numeric &EnemyData::num(const std::string &enemy_name, const std::string &key) {

	if (enemies.size() == 0) { load_enemies(); }

	return std::get<Numeric>(enemies[enemy_name][key]);

}

const std::string &EnemyData::str(const std::string &enemy_name, const std::string &key) {

	if (enemies.size() == 0) { load_enemies(); }

	return std::get<std::string>(enemies[enemy_name][key]);
}

const AttackList &EnemyData::get_attack_list(const std::string &enemy_name) {

	if (enemies.size() == 0) { load_enemies(); }

	return attacks[enemy_name];
}
