#pragma once

#include <cmath>
#include <functional>
#include <random>
#include <string>
#include <vector>
#include <variant>

namespace EnemyData {

	// this namespace gives an easy way to read the YAML of each enemy type.
	// i was originally concerned at the number of string comparisons in this
	// namespace, but according to the internet, a computer from 2010 can do
	// about 1.5 million comparisons (on long strings) in 1ms, so I think this
	// is fine? Like Animation::Animation, the str and num functions are designed
	// to have their results cached.

	class Numeric {
		public:
			typedef std::vector<float>      Choice;
			typedef std::pair<float, float> Range;

			~Numeric(){}
			Numeric(float f)  : val_(f) {}
			Numeric(Range f)  : val_(f) {}
			Numeric(Choice f) : val_(f) {}

			float operator()() const {

				if (std::holds_alternative<Range>(val_)) {

					Range r = std::get<Range>(val_);
					return dist(mt) * std::abs(r.second - r.first) + std::min(r.first, r.second);

				} else if (std::holds_alternative<Choice>(val_)) {

					Choice c = std::get<Choice>(val_);
					return c[(int)std::floor(dist(mt) * c.size())];

				} else {
					return std::get<float>(val_);
				}

			}

		private:
			static std::mt19937 mt;
			static std::uniform_real_distribution<float> dist;

			std::variant<Choice, Range, float> val_;
	};

	const std::string &str(const std::string &enemy_name, const std::string &key);
	Numeric &num(const std::string &enemy_name, const std::string &key);

}
