#include "game.hpp"

#include <iostream>

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "entity/entity.hpp"
#include "entity/octree.hpp"
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
	static auto purple = Material{
		.kind = Material::KIND_LIT_UNTEXED,
		.colour = glm::vec3(0.85f, 0.7f, 1.0f)
	};
	static auto green = Material{
		.kind = Material::KIND_LIT_UNTEXED,
		.colour = glm::vec3(0.7f, 1.0f, 0.7f)
	};
	
	static Mesh3d
		sphereMesh,
		cubeMesh;
	
	static Entity car;
	
	void init() {
		sphereMesh = Mesh3d::load("assets/models/sphere.obj");
		cubeMesh = Mesh3d::load("assets/models/cube.obj");
		
		Scene3d::cam.pos = glm::vec3(0.0f, 5.0f, 15.0f);
		
		auto ground = Entity::create(glm::vec3(0.0f));
		ground.makeKinematic(
			ColliderShape::axisAlignedBox(glm::vec3(100.0f, 1.0f, 100.0f)),
			PhysicsMaterial{.sFrict = 0.5f, .dFrict = 0.4f, .bounciness = 0.5f}
		);
		ground.addMesh(cubeMesh, grey, glm::scale(glm::vec3(100.0f, 1.0f, 100.0f)));
		
		car = Entity::create(glm::vec3(0.0f, 5.0f, 0.0f));
		car.makeDynamic(
			ColliderShape::box(glm::vec3(0.75f, 0.5f, 1.0f)),
			PhysicsMaterial{.sFrict = 0.3f, .dFrict = 0.15f, .bounciness = 0.5f},
			1.0f
		);
		car.addMesh(cubeMesh, purple, glm::scale(glm::vec3(0.75f, 0.5f, 1.0f)));
		car.setUpdateFunc([](Entity e, float dt) {
			auto keysHeld = SDL_GetKeyboardState(nullptr);
			
			//e.addForce(glm::vec3(0.0f, 5.0f, 0.0f));
			
			if (keysHeld[SDL_SCANCODE_W]) {
				e.addForce(e.forward() * 40.0f);
			}
			if (keysHeld[SDL_SCANCODE_S]) {
				e.addForce(e.forward() * -40.0f);
			}
			if (keysHeld[SDL_SCANCODE_A]) {
				e.addForceAt(e.right() * 20.0f, e.pos() + e.forward() * 0.7f + e.right() * 0.5f);
			}
			if (keysHeld[SDL_SCANCODE_D]) {
				e.addForceAt(e.right() * -20.0f, e.pos() + e.forward() * 0.7f + e.right() * -0.5f);
			}
		});
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
		
		glm::vec3 f = car.forward();
		
		glm::vec3 cpos = car.pos() - (car.forward() * 10.0f) + glm::vec3(0.0f, 5.0f, 0.0f);
		float cyaw = std::atan2(-glm::abs(f.x), glm::sqrt(f.y * f.y + f.z * f.z));
		std::cout << glm::degrees(cyaw) << '\n';
		
		Scene3d::cam.pos = cpos;
		Scene3d::cam.yaw = cyaw;
		Scene3d::cam.pitch = glm::radians(20.0f);
		
		//Scene3d::addDebugPoint(cpos, glm::vec3(1.0f, 0.0f, 0.0f), false, 0.0f);
		//Scene3d::addDebugLine(cpos, cpos + glm::vec3(glm::cos(cyaw), 0.0f, glm::sin(cyaw)), glm::vec3(1.0f, 0.0f, 0.0f), false, 0.0f);
	}
}
