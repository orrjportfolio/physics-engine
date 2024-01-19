#pragma once

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "geom.hpp"
#include "gfx3d.hpp"

static inline glm::vec3 playerPos;
static inline float playerRadius = 0.5f;

static inline glm::vec3 spherePos = glm::vec3(-4.5f, 0.0f, 0.0f);
static inline float sphereRadius = 1.0f;

static inline glm::vec3 planePos = glm::vec3(-1.5f, 0.0f, 0.0f);
static inline glm::vec2 planeSize = glm::vec2(2.0f, 2.0f);
static inline glm::mat3 planeRot = glm::identity<glm::mat3>();

static inline glm::vec3 aabbPos = glm::vec3(1.5f, 0.0f, 0.0f);
static inline glm::vec3 aabbSize = glm::vec3(2.0f, 2.0f, 2.0f);

static inline glm::vec3 obbPos = glm::vec3(4.5f, 0.0f, 0.0f);
static inline glm::vec3 obbSize = glm::vec3(2.0f, 2.0f, 2.0f);
static inline glm::mat3 obbRot = glm::identity<glm::mat3>();

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
		
		glm::mat3 mat =
			glm::rotate(-gfx3dCam.yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(-gfx3dCam.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 right = mat * glm::vec3(1.0f, 0.0f, 00.0f);
		glm::vec3 up = mat * glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 back = mat * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		
		if (keysHeld[SDL_SCANCODE_A]) { gfx3dCam.pos -= right * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_D]) { gfx3dCam.pos += right * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_Q]) { gfx3dCam.pos -= up * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_E]) { gfx3dCam.pos += up * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_W]) { gfx3dCam.pos -= back * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_S]) { gfx3dCam.pos += back * 10.0f * dt; }
	}
	else {
		SDL_SetRelativeMouseMode(SDL_FALSE);
		
		if (keysHeld[SDL_SCANCODE_A]) { playerPos.x -= 2.0f * dt; }
		if (keysHeld[SDL_SCANCODE_D]) { playerPos.x += 2.0f * dt; }
		if (keysHeld[SDL_SCANCODE_Q]) { playerPos.y -= 2.0f * dt; }
		if (keysHeld[SDL_SCANCODE_E]) { playerPos.y += 2.0f * dt; }
		if (keysHeld[SDL_SCANCODE_W]) { playerPos.z -= 2.0f * dt; }
		if (keysHeld[SDL_SCANCODE_S]) { playerPos.z += 2.0f * dt; }
	}
	
	float time = SDL_GetTicks() / 1000.0f;
	
	obbRot = glm::rotate(time, glm::vec3(0.5f, 1.0f, 0.5f));
	planeRot = glm::rotate(time, glm::vec3(0.5f, 1.0f, 0.0f)) * glm::rotate(-(float)M_PI / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	
	Overlap overlapSphere = sphereSphereOverlap(playerPos, playerRadius, spherePos, sphereRadius);
	if (overlapSphere.exists) {
		playerPos += overlapSphere.norm * (overlapSphere.depth / 2.0f);
		spherePos -= overlapSphere.norm * (overlapSphere.depth / 2.0f);
	}
	
	Overlap overlapPlane = sphereObbOverlap(playerPos, playerRadius, planePos, planePos - glm::vec3(planeSize / 2.0f, 0.0f), planePos + glm::vec3(planeSize / 2.0f, 0.0f), planeRot, glm::inverse(planeRot));
	if (overlapPlane.exists) {
		playerPos += overlapPlane.norm * (overlapPlane.depth / 2.0f);
		planePos -= overlapPlane.norm * (overlapPlane.depth / 2.0f);
	}
	
	Overlap overlapObb = sphereObbOverlap(playerPos, playerRadius, obbPos, obbPos - (obbSize / 2.0f), obbPos + (obbSize / 2.0f), obbRot, glm::inverse(obbRot));
	if (overlapObb.exists) {
		playerPos += overlapObb.norm * (overlapObb.depth / 2.0f);
		obbPos -= overlapObb.norm * (overlapObb.depth / 2.0f);
	}
	
	Overlap overlapAabb = sphereAabbOverlap(playerPos, playerRadius, aabbPos - (aabbSize / 2.0f), aabbPos + (aabbSize / 2.0f));
	if (overlapAabb.exists) {
		playerPos += overlapAabb.norm * (overlapAabb.depth / 2.0f);
		aabbPos -= overlapAabb.norm * (overlapAabb.depth / 2.0f);
	}
	
	glm::vec3 const white = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 const cyan = glm::vec3(0.0f, 1.0f, 1.0f);
	glm::vec3 const red = glm::vec3(1.0f, 0.0f, 0.0f);
	
	gfx3dQueueDrawMesh(
		&gfx3dSphereMesh,
		&gfx3dWhiteTex,
		(
			overlapSphere.exists ||
			overlapPlane.exists ||
			overlapAabb.exists ||
			overlapObb.exists
		)? red : cyan,
		playerPos,
		glm::vec3(playerRadius),
		glm::identity<glm::mat3>()
	);
	
	gfx3dQueueDrawMesh(
		&gfx3dSphereMesh,
		&gfx3dWhiteTex,
		overlapSphere.exists? red : white,
		spherePos,
		glm::vec3(sphereRadius),
		glm::identity<glm::mat3>()
	);
	
	gfx3dQueueDrawMesh(
		&gfx3dPlaneMesh,
		&gfx3dWhiteTex,
		overlapPlane.exists? red : white,
		planePos,
		glm::vec3(planeSize, 1.0f),
		planeRot
	);
	
	gfx3dQueueDrawMesh(
		&gfx3dCubeMesh,
		&gfx3dWhiteTex,
		overlapAabb.exists? red : white,
		aabbPos,
		aabbSize,
		glm::identity<glm::mat3>()
	);
	
	gfx3dQueueDrawMesh(
		&gfx3dCubeMesh,
		&gfx3dWhiteTex,
		overlapObb.exists? red : white,
		obbPos,
		obbSize,
		obbRot
	);
}
