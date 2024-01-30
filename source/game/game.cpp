#include "game.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "gfx/gfx.hpp"
#include "gfx/scene3d.hpp"

namespace Game {
	static auto white = Material{
		.kind = Material::KIND_LIT_UNTEXED,
		.colour = glm::vec3(1.0f, 1.0f, 1.0f)
	};
	
	static Mesh3d cubeMesh;
	
	glm::vec3 poses[10000];
	
	void init() {
		cubeMesh = Mesh3d::load("assets/models/cube.obj");
		
		Scene3d::cam.pos = glm::vec3(0.0f, 0.0f, 3.0f);
		
		for (int i = 0; i < 10000; i++) {
			poses[i] = glm::vec3(
				(rand() / (float)RAND_MAX) * 100.0f,
				(rand() / (float)RAND_MAX) * 100.0f,
				(rand() / (float)RAND_MAX) * 100.0f
			);
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
		
		static float time = 0.0f;
		time += dt;
		
		auto rot = glm::rotate(time, glm::vec3(0.5f, 1.0f, 0.5f));
		
		Scene3d::addObject(&cubeMesh, &white, glm::mat4(rot));
		
		for (int i = 0; i < 2000; i++) {
			Scene3d::addDebugCube(
				poses[i],
				glm::vec3(1.0f),
				rot,
				glm::vec3(0.0f, 1.0f, 0.0f),
				false,
				0.0f
			);
			
			Scene3d::addDebugPoint(
				poses[i],
				glm::vec3(1.0f, 1.0f, 0.0f),
				true,
				0.0f
			);
			
			Scene3d::addDebugLine(
				glm::vec3(0.0f), poses[i],
				glm::vec3(1.0f, 1.0f, 1.0f),
				true,
				0.0f
			);
			
			Scene3d::addDebugSphere(
				poses[i],
				1.414f,
				rot,
				glm::vec3(0.0f, 0.0f, 1.0f),
				false,
				0.0f
			);
		}
	}
}
