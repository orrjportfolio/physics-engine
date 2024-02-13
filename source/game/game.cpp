#include "game.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "entity/entity.hpp"
#include "gfx/gfx.hpp"
#include "gfx/scene3d.hpp"

namespace Game {
	static auto white = Material{
		.kind = Material::KIND_LIT_UNTEXED,
		.colour = glm::vec3(1.0f)
	};
	static auto grey = Material{
		.kind = Material::KIND_LIT_UNTEXED,
		.colour = glm::vec3(0.5f)
	};
	
	static Mesh3d
		sphereMesh,
		cubeMesh;
	
	void init() {
		sphereMesh = Mesh3d::load("assets/models/sphere.obj");
		cubeMesh = Mesh3d::load("assets/models/cube.obj");
		
		Scene3d::cam.pos = glm::vec3(0.0f, 5.0f, 15.0f);
		
		auto ground = Entity::create(glm::vec3(0.0f));
		ground.makeKinematic(
			ColliderShape::axisAlignedBox(glm::vec3(100.0f, 1.0f, 100.0f)),
			PhysicsMaterial{.sFrict = 0.5f, .dFrict = 0.4f, .bounciness = 0.5f}
		);
		ground.addMesh(cubeMesh, grey, glm::vec3(0.0f), glm::vec3(100.0f, 1.0f, 100.0f));
		
		for (int i = 0; i < 400; i++) {
			int k = rand() % 2;
			auto p = glm::vec3(
				(rand() / (float)RAND_MAX) * 200.0f - 100.0f,
				(rand() / (float)RAND_MAX) * 20.0f + 3.0f,
				(rand() / (float)RAND_MAX) * 200.0f - 100.0f
			);
			
			auto e = Entity::create(p);
			e.makeDynamic(
				k? ColliderShape::sphere(1.0f) : ColliderShape::box(glm::vec3(1.0f)),
				PhysicsMaterial{.sFrict = 0.5f, .dFrict = 0.4f, .bounciness = 0.5f},
				1.0f
			);
			e.addMesh(k? sphereMesh : cubeMesh, white);
		}
	}
	
	void update(float dt) {
		auto keysHeld = SDL_GetKeyboardState(nullptr);
		int mouseRelX, mouseRelY;
		auto mouseButtonsHeld = SDL_GetRelativeMouseState(&mouseRelX, &mouseRelY);
		if (mouseButtonsHeld & SDL_BUTTON_RMASK) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			
			auto dt = 1.0f / 60.0f;
			
			Scene3d::cam.pitch += mouseRelY * 0.5f * dt;
			Scene3d::cam.yaw += mouseRelX * 0.5f * dt;
			
			auto mat = glm::mat3(
				glm::rotate(-Scene3d::cam.yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
				glm::rotate(-Scene3d::cam.pitch, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			
			auto right = mat * glm::vec3(1.0f, 0.0f, 0.0f);
			auto up = mat * glm::vec3(0.0f, 1.0f, 0.0f);
			auto back = mat * glm::vec3(0.0f, 0.0f, 1.0f);
			
			if (keysHeld[SDL_SCANCODE_A]) { Scene3d::cam.pos -= right * 10.0f * dt; }
			if (keysHeld[SDL_SCANCODE_D]) { Scene3d::cam.pos += right * 10.0f * dt; }
			if (keysHeld[SDL_SCANCODE_Q]) { Scene3d::cam.pos -= up * 10.0f * dt; }
			if (keysHeld[SDL_SCANCODE_E]) { Scene3d::cam.pos += up * 10.0f * dt; }
			if (keysHeld[SDL_SCANCODE_W]) { Scene3d::cam.pos -= back * 10.0f * dt; }
			if (keysHeld[SDL_SCANCODE_S]) { Scene3d::cam.pos += back * 10.0f * dt; }
			
		}
		else {
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}
		
	}
}
