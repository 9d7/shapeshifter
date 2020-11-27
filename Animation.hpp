
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <list>
#include <memory>
#include <vector>

namespace Animation {

	struct Static {
		size_t x;
		size_t y;
		size_t w;
		size_t h;
		int    z;
	};

	typedef std::vector<std::pair<float, std::weak_ptr<const Static>>> Dynamic;
	typedef std::variant<std::weak_ptr<const Static>, std::weak_ptr<const Dynamic>> Animation;

	Animation find_static(const std::string &name);
	Animation find_dynamic(const std::string &name);

}
