#include "Numeric.hpp"
#include <chrono>

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
