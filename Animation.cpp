#include "Animation.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "yaml-cpp/yaml.h"
#include "data_path.hpp"

namespace {

	std::unordered_map<std::string, std::shared_ptr<Animation::Dynamic>> dynamic_animations;
	std::unordered_map<std::string, std::shared_ptr<Animation::Static>> static_animations;

	void load_animations() {

		YAML::Node static_yaml = YAML::LoadFile(data_path("sprites.yaml"));

		for (auto it = static_yaml.begin(); it != static_yaml.end(); it++) {
			std::string key = it->first.as<std::string>();
			YAML::Node second = it->second;

			// this is a really ugly way to construct this...but i don't
			// know a better one?
			std::shared_ptr<Animation::Static> sprite = std::make_shared<Animation::Static>(Animation::Static{
				second[0].as<size_t>(),
				second[1].as<size_t>(),
				second[2].as<size_t>(),
				second[3].as<size_t>(),
				second[4].as<int>()
			});

			static_animations.emplace(key, sprite);
		}

		YAML::Node dynamic_yaml = YAML::LoadFile(data_path("animations.yaml"));

		for (auto it = dynamic_yaml.begin(); it != dynamic_yaml.end(); it++) {
			std::string key = it->first.as<std::string>();

			std::shared_ptr<Animation::Dynamic> anim = std::make_shared<Animation::Dynamic>();

			YAML::Node frames = it->second;
			for (auto f_it = frames.begin(); f_it != frames.end(); f_it++) {

				const auto &sprite = static_animations.find(
					f_it->second.as<std::string>()
				);
				if (sprite == static_animations.cend()) {
					throw std::runtime_error("Could not find sprite in animation");
				}

				anim->emplace_back(
					f_it->first.as<float>(),
					sprite->second
				);

			}
		}

	}
}

Animation::Animation Animation::find_dynamic(const std::string &name) {

	if (dynamic_animations.size() == 0 && static_animations.size() == 0) {
		load_animations();
	}

	const auto &search = dynamic_animations.find(name);
	if (search == dynamic_animations.end()) {
		throw std::runtime_error("Could not find_dynamic");
	} else {
		return search->second;
	}

}

Animation::Animation Animation::find_static(const std::string &name) {

	if (dynamic_animations.size() == 0 && static_animations.size() == 0) {
		load_animations();
	}

	const auto &search = static_animations.find(name);
	if (search == static_animations.end()) {
		throw std::runtime_error("Could not find_static");
	} else {
		return search->second;
	}

}
