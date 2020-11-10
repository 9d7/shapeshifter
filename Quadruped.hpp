#include "Prism.hpp"
#include "Scene.hpp"
#include <glm/glm.hpp>
#include <array>
#include <reactphysics3d/constraint/HingeJoint.h>
#include <reactphysics3d/engine/PhysicsCommon.h>
#include <vector>

struct Quadruped {

	struct Leg {
		Prism            *thigh = nullptr;
		Prism            *crus  = nullptr;
	};
	Prism *body;

	std::array<Leg, 4> legs;

	Quadruped(
		Scene               &scene,
		rp3d::PhysicsCommon &common,
		rp3d::PhysicsWorld  *world,
		const glm::vec3     &body_size,
		const glm::vec3     &leg_size,
		std::vector<Prism>  &prisms
	);

	~Quadruped();

};
