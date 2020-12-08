#include "Numeric.hpp"
#include <chrono>
#include <regex>
#include <string>
#include <cstdlib>
#include <stdexcept>

std::mt19937 Numeric::mt ((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
std::uniform_real_distribution<float> Numeric::dist (0.0f, 1.0f);

float Numeric::operator()() const {

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

std::variant<std::string, Numeric> Numeric::parse(const std::string& in) {
	// wow, regex is so pretty and readable. :)
	static std::regex range_regex(R"(^\s*(-?\d*\.?\d+)\s*\.\.\s*(-?\d*\.?\d+)\s*$)");
	static std::regex choice_regex(R"(^\s*((-?\d*\.?\d+)\s*,\s*)+(-?\d*\.?\d+)\s*$)");
	static std::regex full_float_regex(R"(^\s*(-?\d*\.?\d+)\s*$)");
	static std::regex float_regex(R"(-?\d*\.?\d+)");

	std::smatch sm;
	if (std::regex_match(in, sm, range_regex)) { // range

		return Numeric(Numeric::Range(
			(float)std::atof(sm[1].str().c_str()),
			(float)std::atof(sm[2].str().c_str())
		));
	}

	if (std::regex_match(in, choice_regex)) { // choice

		std::string i = in;
		std::regex_iterator<std::string::iterator> rit(i.begin(), i.end(), float_regex);
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
}
