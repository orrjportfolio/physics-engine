#pragma once

#include <SDL2/SDL.h>

#include "entity.hpp"
#include "draw3d.hpp"

static inline EntityId ground;

static void gameInit() {
	cam3d.pos = glm::vec3(0.0f, 3.0f, -20.0f);
	cam3d.yaw = (float)M_PI;
	
	ground = entityCreate(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::identity<glm::mat3>()
	);
	entityAddMesh(
		ground,
		&cubeMesh,
		glm::vec3(0.0f),
		glm::vec3(20.0f, 1.0f, 20.0f),
		&whiteTex,
		glm::vec3(0.5f, 0.5f, 0.5f)
	);
	entityAddStaticAabbBody(
		ground,
		glm::vec3(20.0f, 1.0f, 20.0f),
		0.5f, 0.4f,
		0.25f
	);
	
	auto const addSphere = [](int kind, glm::vec3 pos, float radius) {
		EntityId e = entityCreate(
			pos,
			glm::identity<glm::mat3>()
		);
		if (kind == 0) {
			entityAddMesh(
				e,
				&sphereMesh,
				glm::vec3(0.0f),
				glm::vec3(radius),
				&smileTex,
				glm::vec3(1.0f, 1.0f, 1.0f)
			);
			entityAddSphereBody(
				e,
				radius,
				1.0f,
				0.5f, 0.4f,
				0.25f
			);
		}
		else if (kind == 1) {
			entityAddMesh(
				e,
				&cubeMesh,
				glm::vec3(0.0f),
				glm::vec3(radius, radius, radius * 2.0f),
				&whiteTex,
				glm::vec3(1.0f, 1.0f, 1.0f)
			);
			entityAddObbBody(
				e,
				glm::vec3(radius, radius, radius * 2.0f),
				1.0f,
				0.5f, 0.4f,
				0.25f
			);
		}
		else if (kind == 2) {
			entityAddMesh(
				e,
				&cubeMesh,
				glm::vec3(0.0f),
				glm::vec3(radius, radius, radius * 2.0f),
				&whiteTex,
				glm::vec3(0.6f, 0.8f, 0.7f)
			);
			entityAddAabbBody(
				e,
				glm::vec3(radius, radius, radius * 2.0f),
				1.0f,
				0.5f, 0.4f,
				0.25f
			);
		}
	};
	
	srand(/*0*/6);
	for (int i = 0; i < 40; i++) {
		int kind = rand() % 2;
		glm::vec3 pos = glm::vec3(
			((rand() / (float)RAND_MAX) * 20.0f) - 10.0f,
			((rand() / (float)RAND_MAX) * 20.0f) + 2.0f,
			((rand() / (float)RAND_MAX) * 20.0f) - 10.0f
		);
		addSphere(rand() % 3, pos, 1.0f);
	}
	
	/*addSphere(0, glm::vec3(-2.0f, 5.0f, 0.0f), 1.0f);
	addSphere(0, glm::vec3(2.0f, 10.0f, 0.0f), 1.0f);
	addSphere(1, glm::vec3(2.0f, 5.0f, 0.0f), 1.0f);*/
	
	EntityId player = entityCreate(glm::vec3(0.0f, 2.0f, 0.0f), glm::identity<glm::mat3>());
	entityAddCallbacks(player, EntityCallbacks{
		.onUpdate = [](EntityId e, float dt) {
			Uint8 const *keysHeld = SDL_GetKeyboardState(nullptr);
			
			if (keysHeld[SDL_SCANCODE_SPACE]) {
				if (keysHeld[SDL_SCANCODE_A]) {
					//entityBodies[e.slot].force += glm::vec3(-3.0f, 0.0f, 0.0f);
					entityAddForce(e, glm::vec3(-6.0f, 0.0f, 0.0f));
				}
				if (keysHeld[SDL_SCANCODE_D]) {
					//entityBodies[e.slot].force += glm::vec3(3.0f, 0.0f, 0.0f);
					entityAddForce(e, glm::vec3(6.0f, 0.0f, 0.0f));
				}
				if (keysHeld[SDL_SCANCODE_W]) {
					//entityBodies[e.slot].force += glm::vec3(0.0f, 0.0f, -3.0f);
					entityAddForce(e, glm::vec3(0.0f, 0.0f, -6.0f));
				}
				if (keysHeld[SDL_SCANCODE_S]) {
					//entityBodies[e.slot].force += glm::vec3(0.0f, 0.0f, 3.0f);
					entityAddForce(e, glm::vec3(0.0f, 0.0f, 6.0f));
				}
			}
		}
	});
	entityAddMesh(
		player,
		&sphereMesh,
		glm::vec3(0.0f),
		glm::vec3(0.5f),
		&whiteTex,
		glm::vec3(1.0f, 0.0f, 0.0f)
	);
	entityAddSphereBody(
		player,
		0.5f,
		4.0f,
		0.5f, 0.4f,
		0.5f
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
}
