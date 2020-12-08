#pragma once

#include <vector>
#include <functional>
#include <random>
#include <cmath>
#include <variant>
#include <map>

class Numeric {
	public:

		typedef std::vector<float>      Choice;
		typedef std::pair<float, float> Range;
	
		typedef std::variant<std::string, Numeric> ValueType;
		typedef std::unordered_map<std::string, ValueType> ValueStore;

		~Numeric(){}
		Numeric(float f)  : val_(f) {}
		Numeric(Range f)  : val_(f) {}
		Numeric(Choice f) : val_(f) {}

		float operator()() const;

		static std::variant<std::string, Numeric> parse(const std::string& in);

	private:
		static std::mt19937 mt;
		static std::uniform_real_distribution<float> dist;

		std::variant<Choice, Range, float> val_;
};

