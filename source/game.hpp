#pragma once

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "gfx3d.hpp"

namespace Game {
	static void init() {
		
	}
	
	static void update(float dt) {
		Uint8 const *keysHeld = SDL_GetKeyboardState(nullptr);
		glm::ivec2 mouseVel;
		Uint32 mouseButtons = SDL_GetRelativeMouseState(&mouseVel.x, &mouseVel.y);
		
		if (mouseButtons & SDL_BUTTON_RMASK) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			
			Gfx3d::cam.pitch += mouseVel.y * 0.5f * dt;
			Gfx3d::cam.yaw += mouseVel.x * 0.5f * dt;
			
			glm::vec3 forward =
				glm::rotate(-Gfx3d::cam.yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
				glm::rotate(-Gfx3d::cam.pitch, glm::vec3(1.0f, 0.0f, 0.0f)) *
				glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
			glm::vec3 right = glm::cross(forward, up);
			
			if (keysHeld[SDL_SCANCODE_W]) { Gfx3d::cam.pos += forward * 10.0f * dt; }
			if (keysHeld[SDL_SCANCODE_S]) { Gfx3d::cam.pos -= forward * 10.0f * dt; }
			if (keysHeld[SDL_SCANCODE_D]) { Gfx3d::cam.pos += right * 10.0f * dt; }
			if (keysHeld[SDL_SCANCODE_A]) { Gfx3d::cam.pos -= right * 10.0f * dt; }
		}
		else {
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}
		
		Gfx3d::queueDrawMesh(
			&Gfx3d::sphereMesh,
			&Gfx3d::whiteTex,
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(-3.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::identity<glm::mat3>()
		);
		
		Gfx3d::queueDrawMesh(
			&Gfx3d::planeMesh,
			&Gfx3d::whiteTex,
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 2.0f),
			glm::identity<glm::mat3>()
		);
		
		Gfx3d::queueDrawMesh(
			&Gfx3d::cubeMesh,
			&Gfx3d::whiteTex,
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(3.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 2.0f, 2.0f),
			glm::identity<glm::mat3>()
		);
	}
}
