#pragma once

#include <cmath>
#include <functional>
#include <random>
#include <string>
#include <vector>
#include <variant>
#include "Numeric.hpp"
#include <map>
#include "BulletSequencer.hpp"

namespace EnemyData {

	// this namespace gives an easy way to read the YAML of each enemy type.
	// i was originally concerned at the number of string comparisons in this
	// namespace, but according to the internet, a computer from 2010 can do
	// about 1.5 million comparisons (on long strings) in 1ms, so I think this
	// is fine? Like Animation::Animation, the str and num functions are designed
	// to have their results cached.


	const std::string &str(const std::string &enemy_name, const std::string &key);
	Numeric &num(const std::string &enemy_name, const std::string &key);
	const std::map<float, BulletSequencer> &get_attack_list(const std::string &enemy_name);

}
