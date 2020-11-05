#include "Prism.hpp"
#include <algorithm>
#include <reactphysics3d/body/RigidBody.h>
#include <reactphysics3d/collision/shapes/BoxShape.h>
#include <reactphysics3d/components/RigidBodyComponents.h>
#include <reactphysics3d/engine/PhysicsCommon.h>
#include <reactphysics3d/engine/PhysicsWorld.h>
#include <reactphysics3d/mathematics/Quaternion.h>
#include <reactphysics3d/mathematics/Transform.h>
#include <reactphysics3d/mathematics/Vector3.h>
#include <stdexcept>


Prism::Prism(
	Scene                         &scene,
	rp3d::PhysicsCommon           &common,
	rp3d::PhysicsWorld            *world,
	const glm::vec3               &size,
	const glm::vec3               &position
) {

	auto drawable_it = std::find_if(
		scene.drawables.begin(),
		scene.drawables.end(),
		[](Scene::Drawable d) -> bool {
			return d.transform->name == "Player";
		}
	);
	if (drawable_it == scene.drawables.end()) {
		throw std::runtime_error("Couldn't find cube");
	}

	transform = new Scene::Transform();
	scene.drawables.emplace_back(transform);
	scene.drawables.back().pipeline = drawable_it->pipeline;

	transform->scale = size;
	
	// rp3d
	rp3d::Vector3 rpos = rp3d::Vector3(position.x, position.y, position.z);
	rp3d::Quaternion rquat = rp3d::Quaternion::identity();
	rp3d::Transform rtrans = rp3d::Transform(rpos, rquat);

	rp3d::RigidBody *body = world->createRigidBody(rtrans);
	body->setType(rp3d::BodyType::DYNAMIC);

	const rp3d::Vector3 rsize = rp3d::Vector3(size.x, size.y, size.z);
	shape = common.createBoxShape(rsize);

	collider = body->addCollider(shape, rp3d::Transform::identity());
	body->updateMassPropertiesFromColliders();
	rigid = body;

}

Prism::~Prism() {}

void Prism::update() {

	rp3d::Transform rtrans = rigid->getTransform();
	rp3d::Quaternion rquat = rtrans.getOrientation();
	rp3d::Vector3 rpos = rtrans.getPosition();

	transform->position = glm::vec3(rpos.x, rpos.y, rpos.z);
	transform->rotation = glm::quat(rquat.w, rquat.x, rquat.y, rquat.z);

}
