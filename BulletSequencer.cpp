#include "BulletSequencer.hpp"
#include <cmath>
#include <map>
#include <stdexcept>

std::list<BulletSequencer::ConcreteBulletInfo> BulletSequencer::get_firing_pattern() const {

	std::multimap<float, ConcreteBulletInfo> out;

	float previous_start_time = 0.0f;
	float previous_end_time = 0.0f;
	float current_start_time = 0.0f;
	float current_end_time = 0.0f;

	for (AbstractBulletInfo abi : bullets) {


		int number = (int)std::round(abi.number());

		if (number < 0) {
			throw std::runtime_error("Trying to generate negative bullets");
		}

		if (abi.with_previous) {
			current_start_time = previous_start_time;
		} else {
			current_start_time = previous_end_time;
		}
		current_end_time = current_start_time;

		for (int i = 0; i < number; i++) {

			float time_since_prev = abi.time();
			float time_absolute;

			if (abi.parallel) {
				time_absolute = current_start_time + time_since_prev;
			} else {
				time_absolute = current_end_time + time_since_prev;
			}

			current_end_time = std::max(current_end_time, time_absolute);


			ConcreteBulletInfo cbi {
				time_absolute,
				abi.velocity(),
				abi.angle(),
				abi.color() < 0.5f
			};

			out.emplace(time_absolute, cbi);

		}

		previous_start_time = current_start_time;
		previous_end_time = current_end_time;

	}

	std::list<ConcreteBulletInfo> ret;
	for (auto cbi : out) {
		ret.push_back(cbi.second);
	}

	return ret;

}
