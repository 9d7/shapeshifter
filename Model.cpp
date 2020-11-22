#include "Model.hpp"
#include <memory>

Model::Model(std::weak_ptr<View> view_) : view(view_) {}

void Model::update(float elapsed) {

}
