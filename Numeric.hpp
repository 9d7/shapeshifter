#pragma once

#include "glm/glm.hpp"
#include <yaml-cpp/yaml.h>

#include <vector>
#include <functional>
#include <random>
#include <cmath>
#include <variant>
#include <map>
#include <unordered_map>


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

		// yaml parsing from a string should really go into its own file, but c'est la vie
		static std::variant<std::string, Numeric> parse(const std::string& in);
		static Numeric parse_num(const YAML::Node& in);
		static std::string parse_string(const YAML::Node& in);
		static std::vector<std::variant<std::string, Numeric>> parse_sequence(const YAML::Node& in);
		static glm::vec2 parse_vector(const YAML::Node& in);

	private:
		static std::mt19937 mt;
		static std::uniform_real_distribution<float> dist;

		std::variant<Choice, Range, float> val_;
};

