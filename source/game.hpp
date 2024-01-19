#pragma once

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "gfx3d.hpp"

static void gameInit() {
	
}

static void gameUpdate(float dt) {
	Uint8 const *keysHeld = SDL_GetKeyboardState(nullptr);
	glm::ivec2 mouseVel;
	Uint32 mouseButtons = SDL_GetRelativeMouseState(&mouseVel.x, &mouseVel.y);
	
	if (mouseButtons & SDL_BUTTON_RMASK) {
		SDL_SetRelativeMouseMode(SDL_TRUE);
		
		gfx3dCam.pitch += mouseVel.y * 0.5f * dt;
		gfx3dCam.yaw += mouseVel.x * 0.5f * dt;
		
		glm::vec3 forward =
			glm::rotate(-gfx3dCam.yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(-gfx3dCam.pitch, glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 right = glm::cross(forward, up);
		
		if (keysHeld[SDL_SCANCODE_W]) { gfx3dCam.pos += forward * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_S]) { gfx3dCam.pos -= forward * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_D]) { gfx3dCam.pos += right * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_A]) { gfx3dCam.pos -= right * 10.0f * dt; }
	}
	else {
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
	
	gfx3dQueueDrawMesh(
		&gfx3dSphereMesh,
		&gfx3dWhiteTex,
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(-3.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::identity<glm::mat3>()
	);
	
	gfx3dQueueDrawMesh(
		&gfx3dPlaneMesh,
		&gfx3dWhiteTex,
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 2.0f, 2.0f),
		glm::identity<glm::mat3>()
	);
	
	gfx3dQueueDrawMesh(
		&gfx3dCubeMesh,
		&gfx3dWhiteTex,
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(3.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 2.0f, 2.0f),
		glm::identity<glm::mat3>()
	);
}
