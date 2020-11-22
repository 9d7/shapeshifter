#pragma once

#include <memory>
#include "View.hpp"

class Model {

	public:
		Model(std::weak_ptr<View> view_);

		void update(float elapsed);

	protected:
		std::weak_ptr<View> view;

};
