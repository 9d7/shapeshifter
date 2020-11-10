#include "Quadruped.hpp"

Quadruped::Quadruped(
	Scene               &scene,
	rp3d::PhysicsCommon &common,
	rp3d::PhysicsWorld  *world,
	const glm::vec3     &body_size,
	const glm::vec3     &leg_size,
	std::vector<Prism>  &prisms
) {
	prisms.emplace_back(
		scene,
		common,
		world,
		body_size,
		glm::vec3(
			0.0f,
			0.0f,
			body_size.z + 2.0f * leg_size.z
		)
	);

	body = &(prisms.back());

	glm::vec2 leg_pos = glm::vec2(body_size.x - leg_size.x, body_size.y - leg_size.y);
	std::array<glm::vec2, 4> leg_offsets {
		glm::vec2( leg_pos.x,  leg_pos.y),
		glm::vec2( leg_pos.x, -leg_pos.y),
		glm::vec2(-leg_pos.x,  leg_pos.y),
		glm::vec2(-leg_pos.x, -leg_pos.y)
	};

	for (size_t i = 0; i < 4; i++) {
		(void)(leg_offsets[i]);
	}
}
