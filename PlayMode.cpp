#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <reactphysics3d/reactphysics3d.h>

#include <random>

GLuint hexapod_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > hexapod_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("proto.pnct"));
	hexapod_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > hexapod_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("proto.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = hexapod_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = hexapod_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

Load< Sound::Sample > dusty_floor_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("dusty-floor.opus"));
});

PlayMode::PlayMode() : scene(*hexapod_scene) {

	{ // test reactphysics3d
		// First you need to create the PhysicsCommon object. This is a factory module
		// that you can use to create physics world and other objects. It is also responsible
		// for logging and memory management
		

		// Create a physics world
		world = physicsCommon.createPhysicsWorld();
		
		// Create a rigid body in the world
		reactphysics3d::Vector3 position1(0, 1, 0);
		reactphysics3d::Vector3 position2(0, 1, 10);
		reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
		reactphysics3d::Transform transform1(position1, orientation);
		reactphysics3d::Transform transform2(position2, orientation);
		bodyPlayer = world->createRigidBody(transform1);
		bodyEnemy = world->createRigidBody(transform2);

		// Half extents of the box in the x, y and z directions 
		const reactphysics3d::Vector3 halfExtents(1.0, 1.0, 1.0); 
		
		// Create the box shape 
		reactphysics3d::BoxShape* boxShape = physicsCommon.createBoxShape(halfExtents);
		reactphysics3d::Transform t =  reactphysics3d::Transform::identity(); 
 
		// Add the collider to the rigid body 
		
		collider1 = bodyPlayer->addCollider(boxShape, t);

		// Instantiate a sphere collision shape 
		reactphysics3d::SphereShape* sphereShape = physicsCommon.createSphereShape(1.0f); 
		
		// Relative transform of the collider relative to the body origin 
		//reactphysics3d::Transform transform = Transform::identity(); 
		
		// Add the collider to the rigid body 
		collider2 = bodyEnemy->addCollider(sphereShape, t);

		//floor
		reactphysics3d::Vector3 position3(0.0, -1, 0.0); 
		reactphysics3d::Transform transform3(position3, orientation); 
		bodyFloor = world->createRigidBody(transform3);
		bodyFloor->setType(reactphysics3d::BodyType::STATIC);

		const reactphysics3d::Vector3 floorSize(20.0, 1.0, 20.0); 
		
		// Create the box shape 
		reactphysics3d::BoxShape* floorShape = physicsCommon.createBoxShape(floorSize);
		collider3 = bodyFloor->addCollider(floorShape, t);
	}

	//get pointers to leg for convenience:
	for (auto &transform : scene.transforms) {
		if (transform.name == "Player") player = &transform;
		else if (transform.name == "Sphere") sphere = &transform;
		else if (transform.name == "Plane") plane = &transform;
		else if (transform.name == "PlayerSphere") playerSphere = &transform;
	}
	shifted = player;
	//get pointer to camera for convenience:
	//if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

	//start music loop playing:
	// (note: position will be over-ridden in update())
	leg_tip_loop = Sound::loop_3D(*dusty_floor_sample, 1.0f, glm::vec3(0.0f), 10.0f);
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}  else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.downs += 1;
			space.pressed = true;
			return true;
		} 
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		}  else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.pressed = false;
			return true;
		}
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::angleAxis(motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	const reactphysics3d::decimal timeStep = 1.0f / 60.0f;
	world->update(timeStep);

	// Get the updated position of the body
	const reactphysics3d::Transform& transform1 = bodyPlayer->getTransform();
	const reactphysics3d::Vector3& pos1 = transform1.getPosition();
	const reactphysics3d::Transform& transform2 = bodyEnemy->getTransform();
	const reactphysics3d::Vector3& pos2 = transform2.getPosition();
	const reactphysics3d::Transform& transform3 = bodyFloor->getTransform();
	const reactphysics3d::Vector3& pos3 = transform3.getPosition();
	shifted->position = glm::vec3(pos1.x, pos1.z, pos1.y);
	sphere->position = glm::vec3(pos2.x, pos2.z, pos2.y);
	plane->position = glm::vec3(pos3.x, pos3.z, pos3.y);
	//reactphysics3d::AABB flooraabb = collider3->getWorldAABB();
	//if (collider1->testAABBOverlap(flooraabb)) {
		//printf("collide\n");
	//}

	// Display the position of the body
	//std::cout << "Body Position: (" << pos3.x << ", " << pos3.y << ", " << pos3.z << ")" << std::endl;

	transformTimer += elapsed;

	//move camera:
	{
		//combine inputs into a move:
		constexpr float PlayerSpeed = 30.0f;
		glm::vec2 move = glm::vec2(0.0f);
		if (left.pressed && !right.pressed) move.x =-1.0f;
		if (!left.pressed && right.pressed) move.x = 1.0f;
		if (down.pressed && !up.pressed) move.y =-1.0f;
		if (!down.pressed && up.pressed) move.y = 1.0f;
		if (space.pressed && transformTimer > transformDelay) {
			bodyPlayer->removeCollider(collider1);
			reactphysics3d::Transform t =  reactphysics3d::Transform::identity(); 
			if (isHuman) { //to sphere
				playerSphere->position = shifted->position;
				player->position = glm::vec3(0.0f, 0.0f, -10.0f);
				shifted = playerSphere;
				reactphysics3d::SphereShape* sphereShape = physicsCommon.createSphereShape(1.0f); 
				collider1 = bodyPlayer->addCollider(sphereShape, t);

			} else { //back to human (cube)
				player->position = shifted->position;
				playerSphere->position = glm::vec3(0.0f, 0.0f, -10.0f);
				shifted = player;
				const reactphysics3d::Vector3 halfExtents(1.0, 1.0, 1.0); 
				reactphysics3d::BoxShape* boxShape = physicsCommon.createBoxShape(halfExtents);
				collider1 = bodyPlayer->addCollider(boxShape, t);
			}
			transformDelay = transformTimer + 1.0f;
			isHuman = !isHuman;
		}

		//make it so that moving diagonally doesn't go faster:
		if (move != glm::vec2(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

		reactphysics3d::Vector3 position(pos1.x + move.x, pos1.y, pos1.z + move.y); 
		reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity(); 
		reactphysics3d::Transform newTransform(position, orientation); 
		
		// Move the collision body 
		bodyPlayer->setTransform(newTransform);

		// glm::mat4x3 frame = camera->transform->make_local_to_parent();
		// glm::vec3 right = frame[0];
		// //glm::vec3 up = frame[1];
		// glm::vec3 forward = -frame[2];

		// camera->transform->position += move.x * right + move.y * forward;
	}

	{ //update listener to camera position:
		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 right = frame[0];
		glm::vec3 at = frame[3];
		Sound::listener.set_position_right(at, right, 1.0f / 60.0f);
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
	GL_ERRORS();
}
