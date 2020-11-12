#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include <math.h> 

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

PlayMode::PlayMode() : scene(*hexapod_scene) {

	{ // test reactphysics3d
		// First you need to create the PhysicsCommon object. This is a factory module
		// that you can use to create physics world and other objects. It is also responsible
		// for logging and memory management
		

		// Create a physics world
		rp3d::PhysicsWorld::WorldSettings settings;
		settings.gravity = rp3d::Vector3(0.0f, 0.0f, -9.81f);
		world = physicsCommon.createPhysicsWorld(settings); 
		// Register your event listener class 
		world->setEventListener(&listener);
		
		// Create a rigid body in the world
		reactphysics3d::Vector3 position1(0, -10, 2);
		reactphysics3d::Vector3 position2(0, 1, 2);
		reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
		reactphysics3d::Transform transform1(position1, orientation);
		reactphysics3d::Transform transform2(position2, orientation);
		body_player = world->createRigidBody(transform1);
		body_enemy = world->createRigidBody(transform2);

		// Half extents of the box in the x, y and z directions 
		const reactphysics3d::Vector3 halfExtents(1.0, 1.0, 1.0); 
		
		// Create the box shape 
		reactphysics3d::BoxShape* boxShape = physicsCommon.createBoxShape(halfExtents);
		reactphysics3d::Transform t =  reactphysics3d::Transform::identity(); 
 
		// Add the collider to the rigid body 
		
		collider1 = body_player->addCollider(boxShape, t);

		// Instantiate a sphere collision shape 
		reactphysics3d::SphereShape* sphereShape = physicsCommon.createSphereShape(1.0f); 
		
		// Relative transform of the collider relative to the body origin 
		//reactphysics3d::Transform transform = Transform::identity(); 
		
		// Add the collider to the rigid body 
		collider2 = body_enemy->addCollider(sphereShape, t);

		//floor
		level_1();
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
	printf("%f %f\n", camera->transform->position.x, camera->transform->position.x);
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	SDL_KeyCode key = (SDL_KeyCode)evt.key.keysym.sym;
	if (evt.type == SDL_KEYDOWN) {
		if (key == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		}
		if (buttons.find(key) != buttons.end()) {
			//buttons[key].downs += 1;
			buttons[key].pressed = true;
			return true;
		}
	}
	else if (evt.type == SDL_KEYUP) {
		if (buttons.find(key) != buttons.end()) {
			buttons[key].pressed = false;
			return true;
		}
	}	
	else if (evt.type == SDL_MOUSEBUTTONDOWN) {
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
			auto rx = glm::angleAxis(motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f));
			auto ry = glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f));
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::angleAxis(motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			// printf("%f %f %f \n", rot.x, rot.y, rot.z);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	const reactphysics3d::decimal timeStep = 1.0f / 60.0f;
	world->update(timeStep);

	// Get the updated position of the body
	reactphysics3d::Transform transform_player = body_player->getTransform();
	reactphysics3d::Vector3 position_player = transform_player.getPosition(); // default 0,y,0
	reactphysics3d::Transform transform_enemy = body_enemy->getTransform();
	reactphysics3d::Vector3 position_enemy = transform_enemy.getPosition(); // default 0,y,10
	//const reactphysics3d::Transform& transform_floor = body_floor->getTransform();
	//const reactphysics3d::Vector3& position_floor = transform_floor.getPosition();
	shifted->position = glm::vec3(position_player.x, position_player.y, position_player.z);
	sphere->position = glm::vec3(position_enemy.x, position_enemy.y, position_enemy.z);
	//plane->position = glm::vec3(position_floor.x, position_floor.y, position_floor.z);
	//reactphysics3d::AABB flooraabb = collider3->getWorldAABB();
	//if (collider1->testAABBOverlap(flooraabb)) {
		//printf("collide\n");
	//}

	// Display the position of the body
	//std::cout << "Body Position: (" << position_floor.x << ", " << position_floor.y << ", " << position_floor.z << ")" << std::endl;

	transformTimer += elapsed;

	// Check if player is within a certain distance with enemy
	//std::cout << (position_enemy - position_player).length() << "\n";
	//std::cout << position_enemy.to_string() << "\n";
	handle_triggers();
	handle_dynamic_collisions();

	// Reset if reset button pressed
	if (buttons[SDLK_k].pressed) {
		is_dead = false;
		position_enemy = reactphysics3d::Vector3(0.0f, -10.0f, 2);
		position_player = reactphysics3d::Vector3(0.0f, 1.0f, 2);
	}

	//move camera:
	{
		//combine inputs into a move:
		constexpr float PlayerSpeed = 20.0f;
		reactphysics3d::Vector3 move;

		if (is_human) {
			move = move_character(buttons[SDLK_w].pressed, buttons[SDLK_s].pressed, buttons[SDLK_a].pressed, buttons[SDLK_d].pressed);
		}
		else {
			move = move_character(buttons[SDLK_q].pressed, buttons[SDLK_e].pressed, buttons[SDLK_r].pressed, buttons[SDLK_t].pressed);
		}

		if (buttons[SDLK_SPACE].pressed && transformTimer > transformDelay) {
			body_player->removeCollider(collider1);
			reactphysics3d::Transform t =  reactphysics3d::Transform::identity(); 
			if (is_human) { //to sphere
				playerSphere->position = shifted->position;
				player->position = glm::vec3(0.0f, 0.0f, -10.0f);
				shifted = playerSphere;
				reactphysics3d::SphereShape* sphereShape = physicsCommon.createSphereShape(1.0f); 
				collider1 = body_player->addCollider(sphereShape, t);

			} else { //back to human (cube)
				player->position = shifted->position;
				playerSphere->position = glm::vec3(0.0f, 0.0f, -10.0f);
				shifted = player;
				const reactphysics3d::Vector3 halfExtents(1.0, 1.0, 1.0); 
				reactphysics3d::BoxShape* boxShape = physicsCommon.createBoxShape(halfExtents);
				collider1 = body_player->addCollider(boxShape, t);
			}
			transformDelay = transformTimer + 1.0f;
			is_human = !is_human;
		}

		//make it so that moving diagonally doesn't go faster:
		if (move != reactphysics3d::Vector3::zero()) {
			move.normalize();
			move = move * PlayerSpeed * elapsed;
		}
		camera->transform->position.x += move.x;
		camera->transform->position.y += move.y;
		glm::mat4 view = glm::lookAt(camera->transform->position, shifted->position, glm::vec3(0.0, 1.0, 0.0));
		camera->transform->rotation = glm::conjugate(glm::quat_cast(view));

		// If player is currently human, enemy goes towards them
		if (is_human) {
			reactphysics3d::Vector3 move_enemy = position_player - position_enemy;
			move_enemy.normalize();
			move_enemy = move_enemy * PlayerSpeed * .25f * elapsed;
			transform_enemy.setPosition(position_enemy + move_enemy);
		}

		
		//const reactphysics3d::Vector3& position(position_player.x + move.x, position_player.y, position_player.z + move.y); 
		//reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity(); 
		//reactphysics3d::Transform new_transform_player(position_player + move, transform_player.getOrientation()); 
		//reactphysics3d::Vector3 position = position_player + move;
		transform_player.setPosition(position_player + move);
		// Move the collision body 
		body_player->setTransform(transform_player);
		body_enemy->setTransform(transform_enemy);

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
		
		std::string const instructions = controls_text[!is_human];

		constexpr float H = 0.09f;
		lines.draw_text(instructions,
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(1.4f * H, 0.0f, 0.0f), glm::vec3(0.0f, 1.4f * H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text(instructions,
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + 0.1f * H + ofs, 0.0),
			glm::vec3(1.4f * H, 0.0f, 0.0f), glm::vec3(0.0f, 1.4f * H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));

		if (is_dead) {
			lines.draw_text("YOU DIED",
				glm::vec3(-7.0f * H,0.0f, 0.0f),
				glm::vec3(5.0f * H, 0.0f, 0.0f), glm::vec3(0.0f, 5.0f * H, 0.0f),
				glm::u8vec4(0xff, 0x00, 0x00, 0x00));
			lines.draw_text("YOU DIED",
				glm::vec3(-7.0f * H + ofs, 0.0f + ofs, 0.0f),
				glm::vec3(5.0f * H, 0.0f, 0.0f), glm::vec3(0.0f, 5.0f * H, 0.0f),
				glm::u8vec4(0x4f, 0x0f, 0x0f, 0x00));
		}
		if (is_human) {
			lines.draw_text("NORMAL",
				glm::vec3(-aspect + 0.1f * H, 0.7f - 0.1f * H, 0.0f),
				glm::vec3(3.0f * H, 0.0f, 0.0f), glm::vec3(0.0f, 3.0f * H, 0.0f),
				glm::u8vec4(0x00, 0x00, 0xff, 0x00));
			lines.draw_text("NORMAL",
				glm::vec3(-aspect + 0.1f * H + ofs, .7f - 0.1f * H - ofs, 0.0f),
				glm::vec3(3.0f * H, 0.0f, 0.0f), glm::vec3(0.0f, 3.0f * H, 0.0f),
				glm::u8vec4(0xaf, 0xaf, 0xff, 0x00));
		}
		else {
			lines.draw_text("SHAPESHIFTED",
				glm::vec3(-aspect + 0.1f * H, 0.7f - 0.1f * H, 0.0f),
				glm::vec3(3.0f * H, 0.0f, 0.0f), glm::vec3(0.0f, 3.0f * H, 0.0f),
				glm::u8vec4(0x00, 0xff, 0x00, 0x00));
			lines.draw_text("SHAPESHIFTED",
				glm::vec3(-aspect + 0.1f * H + ofs, .7f - 0.1f * H - ofs, 0.0f),
				glm::vec3(3.0f * H, 0.0f, 0.0f), glm::vec3(0.0f, 3.0f * H, 0.0f),
				glm::u8vec4(0x0f, 0x4f, 0x0f, 0x00));
		}
	}
	GL_ERRORS();
}

reactphysics3d::Vector3 PlayMode::move_character(bool up, bool down, bool left, bool right) {
	reactphysics3d::Vector3 move = reactphysics3d::Vector3::zero();
	if (left && !right) move.x = -1.0f;
	if (!left && right) move.x = 1.0f;
	if (down && !up) move.y = -1.0f;
	if (!down && up) move.y = 1.0f;

	return move;

}

void PlayMode::handle_triggers() {

	for (size_t i = 0; i < listener.trigs.size(); i++) {
		if (listener.trigs[i] == collider1) continue;
		if (listener.trigs[i] == current_map.flagcol) {
			level_cleared = true;
			printf("cleared!\n");
			return ;
		} else {
			is_dead = true;
		}
	}
}

void PlayMode::handle_dynamic_collisions() {
	for (size_t i = 0; i < listener.cols.size(); i += 2) {
		if (current_map.walkables.find(listener.cols[i]) == current_map.walkables.end() && listener.cols[i+1] == collider1 ||
		    current_map.walkables.find(listener.cols[i+1]) == current_map.walkables.end() && listener.cols[i] == collider1) {
			if (is_human) {
				is_dead = true;
			}
		}
	}
}

void PlayMode::create_rigid_box(reactphysics3d::Vector3 pos, reactphysics3d::Vector3 size, bool trigger) {
    reactphysics3d::Transform transform1(pos, rp3d::Quaternion::identity()); 
    rp3d::RigidBody *body = world->createRigidBody(transform1);
    body->setType(rp3d::BodyType::STATIC);
    reactphysics3d::BoxShape* shape = physicsCommon.createBoxShape(size);
    rp3d::Collider *col = body->addCollider(shape, rp3d::Transform::identity());

	if (trigger) {
		col->setIsTrigger(true);
		current_map.triggers[col] = body;
	} else {
		current_map.walkables[col] = body;
	}
}

void PlayMode::level_1() {
    create_rigid_box(rp3d::Vector3(0.0, 0, -1), rp3d::Vector3(20.0, 20.0, 1.0), false);
    create_rigid_box(rp3d::Vector3(0.0, 25, -1), rp3d::Vector3(5.0, 5.0, 1.0), false);
	create_rigid_box(rp3d::Vector3(0.0, 35, -1), rp3d::Vector3(20.0, 5.0, 1.0), false);
	create_rigid_box(rp3d::Vector3(15, 45, -1), rp3d::Vector3(5.0, 5.0, 1.0), false);
	create_rigid_box(rp3d::Vector3(0, 55, -1), rp3d::Vector3(20.0, 5.0, 1.0), false);
	create_rigid_box(rp3d::Vector3(-15, 65, -1), rp3d::Vector3(5.0, 5.0, 1.0), false);
	create_rigid_box(rp3d::Vector3(-10, 75, -1), rp3d::Vector3(10.0, 5.0, 1.0), false);
	create_rigid_box(rp3d::Vector3(10, 75, -1), rp3d::Vector3(10.0, 10.0, 1.0), false);

	create_rigid_box(rp3d::Vector3(0, 25, -8), rp3d::Vector3(20.0, 5.0, 5), true);
	create_rigid_box(rp3d::Vector3(0, 45, -8), rp3d::Vector3(20.0, 5.0, 5), true);
	create_rigid_box(rp3d::Vector3(0, 65, -8), rp3d::Vector3(20.0, 5.0, 5), true);

	reactphysics3d::Transform transform1(rp3d::Vector3(10, 75, 3), rp3d::Quaternion::identity()); 
    rp3d::RigidBody *body = world->createRigidBody(transform1);
    body->setType(rp3d::BodyType::STATIC);
    reactphysics3d::BoxShape* shape = physicsCommon.createBoxShape(rp3d::Vector3(1, 1, 3));
    rp3d::Collider *col = body->addCollider(shape, rp3d::Transform::identity());
	col->setIsTrigger(true);
	current_map.flagrb = body;
	current_map.flagcol = col;
}
