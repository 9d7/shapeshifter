#pragma once

#include <string>
namespace EnemyData {

	// this namespace gives an easy way to read the YAML of each enemy type.
	// i was originally concerned at the number of string comparisons in this
	// namespace, but according to the internet, a computer from 2010 can do
	// about 1.5 million comparisons (on long strings) in 1ms, so I think this
	// is fine.

	std::string str(const std::string &enemy_name, const std::string &key);
	float num(const std::string &enemy_name, const std::string &key);

}
