#include "Scene.hpp"
#include "glm/fwd.hpp"
#include <reactphysics3d/body/RigidBody.h>
#include <reactphysics3d/collision/Collider.h>
#include <reactphysics3d/collision/shapes/BoxShape.h>
#include <reactphysics3d/engine/PhysicsCommon.h>

namespace rp3d = reactphysics3d;
struct Prism {

	Prism(
		Scene               &scene,
		rp3d::PhysicsCommon &common,
		rp3d::PhysicsWorld  *world,
		const glm::vec3     &size,
		const glm::vec3     &position
	);
	~Prism();

	void update();

	rp3d::RigidBody  *rigid;
	rp3d::Collider   *collider;
	rp3d::BoxShape   *shape;
	Scene::Transform *transform;

};
