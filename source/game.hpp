#pragma once

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "geom.hpp"
#include "gfx3d.hpp"

static inline glm::vec3 playerPos = glm::vec3(0.0f, 0.0f, 3.0f);
static inline glm::vec3 playerSize = glm::vec3(1.0f, 1.0f, 1.0f);

static inline glm::vec3 spherePos = glm::vec3(-3.0f, 0.0f, 0.0f);
static inline float sphereRadius = 1.0f;

static inline glm::vec3 aabbPos = glm::vec3(0.0f, 0.0f, 0.0f);
static inline glm::vec3 aabbSize = glm::vec3(2.0f, 2.0f, 2.0f);

static inline glm::vec3 obbPos = glm::vec3(3.0f, 0.0f, 0.0f);
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
	
	Overlap overlapSphere = sphereAabbOverlap(spherePos, sphereRadius, playerPos - (playerSize / 2.0f), playerPos + (playerSize / 2.0f));
	if (overlapSphere.exists) {
		spherePos += overlapSphere.norm * (overlapSphere.depth / 2.0f);
		playerPos -= overlapSphere.norm * (overlapSphere.depth / 2.0f);
	}
	
	Overlap overlapAabb = aabbAabbOverlap(playerPos - (playerSize / 2.0f), playerPos + (playerSize / 2.0f), aabbPos - (aabbSize / 2.0f), aabbPos + (aabbSize / 2.0f));
	if (overlapAabb.exists) {
		playerPos += overlapAabb.norm * (overlapAabb.depth / 2.0f);
		aabbPos -= overlapAabb.norm * (overlapAabb.depth / 2.0f);
	}
	
	glm::vec3 playerVerts[8], obbVerts2[8];
	aabbVerts(playerPos - (playerSize / 2.0f), playerPos + (playerSize / 2.0f), playerVerts);
	obbVerts(obbPos, obbSize, obbRot, obbVerts2);
	Overlap overlapObb = obbObbOverlap(playerPos, playerVerts, obbPos, obbVerts2);
	if (overlapObb.exists) {
		playerPos += overlapObb.norm * (overlapObb.depth / 2.0f);
		obbPos -= overlapObb.norm * (overlapObb.depth / 2.0f);
	}
	aabbVerts(playerPos - (playerSize / 2.0f), playerPos + (playerSize / 2.0f), playerVerts);
	obbVerts(obbPos, obbSize, obbRot, obbVerts2);
	
	glm::vec3 const white = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 const cyan = glm::vec3(0.0f, 1.0f, 1.0f);
	glm::vec3 const red = glm::vec3(1.0f, 0.0f, 0.0f);
	
	gfx3dQueueDrawMesh(
		&gfx3dCubeMesh,
		&gfx3dWhiteTex,
		(
			overlapSphere.exists ||
			overlapAabb.exists ||
			overlapObb.exists
		)? red : cyan,
		playerPos,
		playerSize,
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
