#pragma once

#include <vector>
#include <functional>
#include <variant>
#include <random>
#include <cmath>

class Numeric {
	public:
		typedef std::vector<float>      Choice;
		typedef std::pair<float, float> Range;

		~Numeric(){}
		Numeric(float f)  : val_(f) {}
		Numeric(Range f)  : val_(f) {}
		Numeric(Choice f) : val_(f) {}

		float operator()() const;


	private:
		static std::mt19937 mt;
		static std::uniform_real_distribution<float> dist;

		std::variant<Choice, Range, float> val_;
};

