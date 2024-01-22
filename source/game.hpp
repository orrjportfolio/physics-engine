#pragma once

#include <SDL2/SDL.h>

#include "entity.hpp"
#include "draw3d.hpp"

static inline EntityId ground;
static inline EntityId testEntity;

static void gameInit() {
	ground = entityCreate(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::identity<glm::mat3>()
	);
	entityAddMesh(ground, &cubeMesh, glm::vec3(0.0f), glm::vec3(20.0f, 1.0f, 20.0f), &whiteTex, glm::vec3(0.5f, 0.5f, 0.5f));
	entityAddStaticAabbBody(ground, glm::vec3(20.0f, 1.0f, 20.0f), 0.5f, 0.5f);
	
	testEntity = entityCreate(
		glm::vec3(0.0f, 5.0f, 0.0f),
		glm::identity<glm::mat3>()
	);
	entityAddMesh(
		testEntity,
		&sphereMesh,
		glm::vec3(0.0f),
		glm::vec3(1.0f),
		&smileTex,
		glm::vec3(1.0f, 1.0f, 1.0f)
	);
	entityAddSphereBody(
		testEntity,
		1.0f,
		1.0f,
		0.5f, 0.5f
	);
}

static void gameUpdate(float dt) {
	Uint8 const *keysHeld = SDL_GetKeyboardState(nullptr);
	glm::ivec2 mouseVel;
	Uint32 mouseButtonsHeld = SDL_GetRelativeMouseState(&mouseVel.x, &mouseVel.y);
	if (mouseButtonsHeld &= SDL_BUTTON_RMASK) {
		SDL_SetRelativeMouseMode(SDL_TRUE);
		
		SDL_SetRelativeMouseMode(SDL_TRUE);
		
		cam3d.pitch += mouseVel.y * 0.5f * dt;
		cam3d.yaw += mouseVel.x * 0.5f * dt;
		
		glm::mat3 mat =
			glm::rotate(-cam3d.yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(-cam3d.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 right = mat * glm::vec3(1.0f, 0.0f, 00.0f);
		glm::vec3 up = mat * glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 back = mat * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		
		if (keysHeld[SDL_SCANCODE_A]) { cam3d.pos -= right * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_D]) { cam3d.pos += right * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_Q]) { cam3d.pos -= up * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_E]) { cam3d.pos += up * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_W]) { cam3d.pos -= back * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_S]) { cam3d.pos += back * 10.0f * dt; }
	}
	else {
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
	
	if (keysHeld[SDL_SCANCODE_UP]) {
		entityBodies[testEntity.slot].torque += glm::vec3(10.0f, 0.0f, 0.0f);
	}
	if (keysHeld[SDL_SCANCODE_DOWN]) {
		entityBodies[testEntity.slot].torque -= glm::vec3(10.0f, 0.0f, 0.0f);
	}
}
