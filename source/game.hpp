#pragma once

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "geom.hpp"
#include "gfx3d.hpp"

static inline glm::vec3 playerNorm = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
static inline float playerDist = -5.0f;

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
		
		if (keysHeld[SDL_SCANCODE_Q]) { playerDist -= 2.0f * dt; }
		if (keysHeld[SDL_SCANCODE_E]) { playerDist += 2.0f * dt; }
	}
	
	float time = SDL_GetTicks() / 1000.0f;
	
	obbRot = glm::rotate(time, glm::vec3(0.5f, 1.0f, 0.5f));
	
	size_t numContactPoints = 0;
	glm::vec3 contactPoints[128];
	
	Overlap overlapSphere = spherePlaneOverlap(spherePos, sphereRadius, playerNorm, playerDist);
	if (overlapSphere.exists) {
		spherePos += overlapSphere.norm * overlapSphere.depth;
		contactPoints[numContactPoints++] = spherePlaneContact(spherePos, sphereRadius, playerNorm, playerDist);
	}
	
	Overlap overlapAabb = aabbPlaneOverlap(aabbPos, aabbSize, playerNorm, playerDist);
	if (overlapAabb.exists) {
		aabbPos += overlapAabb.norm * overlapAabb.depth;
		
		glm::vec3 aabbVerts2[8];
		aabbVerts(aabbPos - (aabbSize / 2.0f), aabbPos + (aabbSize / 2.0f), aabbVerts2);
		size_t n = obbPlaneContacts(aabbVerts2, playerNorm, playerDist, contactPoints + numContactPoints);
		numContactPoints += n;
	}
	
	glm::vec3 obbVerts2[8];
	obbVerts(obbPos, obbSize, obbRot, obbVerts2);
	Overlap overlapObb = obbPlaneOverlap(obbVerts2, playerNorm, playerDist);
	if (overlapObb.exists) {
		obbPos += overlapObb.norm * overlapObb.depth;
		
		obbVerts(obbPos, obbSize, obbRot, obbVerts2);
		size_t n = obbPlaneContacts(obbVerts2, playerNorm, playerDist, contactPoints + numContactPoints);
		numContactPoints += n;
	}
	
	for (size_t i = 0; i < numContactPoints; i++) {
		gfx3dQueueDrawMesh(
			&gfx3dSphereMesh,
			&gfx3dWhiteTex,
			glm::vec3(0.0f, 0.0f, 1.0f),
			contactPoints[i],
			glm::vec3(0.1f),
			glm::identity<glm::mat3>()
		);
	}
	
	glm::vec3 const white = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 const cyan = glm::vec3(0.0f, 1.0f, 1.0f);
	glm::vec3 const red = glm::vec3(1.0f, 0.0f, 0.0f);
	
	glm::vec3 defaultNorm = glm::vec3(0.0f, 0.0f, 1.0f);
	float playerAngle = glm::acos(glm::dot(defaultNorm, playerNorm));
	glm::vec3 playerAxis = glm::cross(defaultNorm, playerNorm);
	//glm::mat3 playerMat = 
	
	gfx3dQueueDrawMesh(
		&gfx3dPlaneMesh,
		&gfx3dWhiteTex,
		(
			overlapSphere.exists ||
			overlapAabb.exists ||
			overlapObb.exists
		)? red : cyan,
		playerNorm * playerDist,
		glm::vec3(200.0f),
		glm::rotate(playerAngle, playerAxis)
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
