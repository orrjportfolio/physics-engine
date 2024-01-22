#pragma once

#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

#include "geom.hpp"

enum BodyKind {
	BODY_SPHERE,
	BODY_AABB,
	BODY_OBB
};

struct Body {
	BodyKind kind;
	union {
		float radius;
		struct {
			glm::vec3 size;
		};
	};
	
	bool isStatic;
	
	float volume;
	float mass, invMass;
	glm::mat3 initInertiaTensor;
	float sFriction, dFriction;
	
	glm::vec3 vel;
	glm::vec3 rotMom;
	
	glm::mat3 inertiaTensor;
	glm::vec3 rotVel;
	
	glm::vec3 force, torque;
};

static Body bodyCreateSphere(
	glm::mat3 rot,
	float radius,
	float density,
	float sFriction, float dFriction
) {
	float volume = (4.0f * (float)M_PI * (radius * radius * radius)) / 3.0f;
	float mass = density * volume;
	glm::mat3 initInertiaTensor = glm::mat3((2.0f * mass * (radius * radius)) / 5.0f);
	
	return Body{
		.kind = BODY_SPHERE,
		.radius = radius,
		
		.isStatic = false,
		
		.volume = volume,
		.mass = mass,
		.invMass = 1.0f / mass,
		.initInertiaTensor = initInertiaTensor,
		.sFriction = sFriction,
		.dFriction = dFriction,
		
		.vel = glm::vec3(0.0f),
		.rotMom = glm::vec3(0.0f),
		
		.inertiaTensor = rot * initInertiaTensor * glm::transpose(rot),
		.rotVel = glm::vec3(0.0f),
		
		.force = glm::vec3(0.0f),
		.torque = glm::vec3(0.0f)
	};
}

static Body bodyCreateAabb(
	glm::vec3 size,
	float density,
	float sFriction, float dFriction
) {
	float volume = size.x * size.y * size.z;
	float mass = density * volume;
	
	return Body{
		.kind = BODY_AABB,
		.size = size,
		
		.isStatic = false,
		
		.volume = volume,
		.mass = mass,
		.invMass = 1.0f / mass,
		.sFriction = sFriction,
		.dFriction = dFriction,
		
		.vel = glm::vec3(0.0f),
		
		.rotVel = glm::vec3(0.0f),
		
		.force = glm::vec3(0.0f),
	};
}

static Body bodyCreateObb(
	glm::mat3 rot,
	glm::vec3 size,
	float density,
	float sFriction, float dFriction
) {
	float volume = size.x * size.y * size.z;
	float mass = density * volume;
	glm::mat3 initInertiaTensor = glm::mat3(
		glm::vec3((mass * ((size.y * size.y) + (size.z * size.z))) / 12.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, (mass * ((size.x * size.x) + (size.y * size.y))) / 12.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, (mass * ((size.x * size.x) + (size.z * size.z))) / 12.0f)
	);
	
	return Body{
		.kind = BODY_OBB,
		.size = size,
		
		.isStatic = false,
		
		.volume = volume,
		.mass = mass,
		.invMass = 1.0f / mass,
		.initInertiaTensor = initInertiaTensor,
		.sFriction = sFriction,
		.dFriction = dFriction,
		
		.vel = glm::vec3(0.0f),
		.rotMom = glm::vec3(0.0f),
		
		.inertiaTensor = rot * initInertiaTensor * glm::transpose(rot),
		.rotVel = glm::vec3(0.0f),
		
		.force = glm::vec3(0.0f),
		.torque = glm::vec3(0.0f)
	};
}

static Body bodyCreateStaticSphere(
	glm::mat3 rot,
	float radius,
	float sFriction, float dFriction
) {
	return Body{
		.kind = BODY_SPHERE,
		.radius = radius,
		
		.isStatic = true,
		
		.volume = (4.0f * (float)M_PI * (radius * radius * radius)) / 3.0f,
		.invMass = 0.0f,
		.sFriction = sFriction,
		.dFriction = dFriction
	};
}

static Body bodyCreateStaticAabb(
	glm::vec3 size,
	float sFriction, float dFriction
) {
	return Body{
		.kind = BODY_AABB,
		.size = size,
		
		.isStatic = true,
		
		.volume = size.x * size.y * size.z,
		.invMass = 0.0f,
		.sFriction = sFriction,
		.dFriction = dFriction
	};
}

static Body bodyCreateStaticObb(
	glm::vec3 size,
	float sFriction, float dFriction
) {
	return Body{
		.kind = BODY_OBB,
		.size = size,
		
		.isStatic = true,
		
		.volume = size.x * size.y * size.z,
		.invMass = 0.0f,
		.sFriction = sFriction,
		.dFriction = dFriction
	};
}

static void bodySimulate(glm::vec3 *bodyPos, glm::mat3 *bodyRot, Body *body, glm::vec3 gravity, float dt) {
	if (body->isStatic) { return; }
	
	*bodyPos += body->vel * dt;
	
	glm::vec3 accel = body->force / body->mass;
	body->vel += (accel + gravity) * dt;
	
	*bodyRot += dt * (dt * glm::mat3(
		glm::vec3(0.0f, body->rotVel.z, -body->rotVel.y),
		glm::vec3(-body->rotVel.z, 0.0f, -body->rotVel.x),
		glm::vec3(body->rotVel.y, -body->rotVel.x, 0.0f)
	)) * *bodyRot;
	*bodyRot = glm::orthonormalize(*bodyRot);
	
	body->rotMom += body->torque * dt;
	
	body->inertiaTensor = *bodyRot * body->initInertiaTensor * glm::transpose(*bodyRot);
	
	body->rotVel = glm::inverse(body->inertiaTensor) * body->rotMom;
	
	body->force = glm::vec3(0.0f);
	body->torque = glm::vec3(0.0f);
}

static void bodyCollide(
	glm::vec3 *aPos, glm::mat3 *aRot, Body *a,
	glm::vec3 *bPos, glm::mat3 *bRot, Body *b
) {
	if (a->isStatic && b->isStatic) { return; }
	
	glm::vec3 aVerts[8], bVerts[8];
	
	if (a->kind == BODY_AABB) {
		aabbVerts(*aPos + (a->size / 2.0f), *aPos - (a->size / 2.0f), aVerts);
	}
	else if (a->kind == BODY_OBB) {
		obbVerts(*aPos, a->size, *aRot, aVerts);
	}
	
	if (b->kind == BODY_AABB) {
		aabbVerts(*bPos + (b->size / 2.0f), *bPos - (b->size / 2.0f), bVerts);
	}
	else if (b->kind == BODY_OBB) {
		obbVerts(*bPos, b->size, *bRot, bVerts);
	}
	
	Overlap overlap;
	if (a->kind == BODY_SPHERE) {
		if (b->kind == BODY_SPHERE) {
			overlap = sphereSphereOverlap(*aPos, a->radius, *bPos, b->radius);
		}
		else if (b->kind == BODY_AABB) {
			overlap = sphereAabbOverlap(*aPos, a->radius, *bPos - (b->size / 2.0f), *bPos + (b->size / 2.0f));
		}
		else if (b->kind == BODY_OBB) {
			glm::mat3 bRotInv = glm::inverse(*bRot);
			overlap = sphereObbOverlap(*aPos, a->radius, *bPos, *bPos - (b->size / 2.0f), *bPos + (b->size / 2.0f), bRot, &bRotInv);
		}
	}
	else if (a->kind == BODY_AABB) {
		if (b->kind == BODY_SPHERE) {
			overlap = sphereAabbOverlap(*bPos, b->radius, *aPos - (a->size / 2.0f), *aPos + (a->size / 2.0f));
			overlap.norm = -overlap.norm;
		}
		else if (b->kind == BODY_AABB) {
			overlap = aabbAabbOverlap(*aPos - (a->size / 2.0f), *aPos + (a->size / 2.0f), *bPos - (b->size / 2.0f), *bPos + (b->size / 2.0f));
		}
		else if (b->kind == BODY_OBB) {
			glm::mat3 bRotInv = glm::inverse(*bRot);
			overlap = obbObbOverlap(*aPos, aVerts, *bPos, bVerts);
		}
	}
	else if (a->kind == BODY_OBB) {
		glm::mat3 aRotInv = glm::inverse(*aRot);
		
		if (b->kind == BODY_SPHERE) {
			overlap = sphereObbOverlap(*bPos, b->radius, *aPos, *aPos - (a->size / 2.0f), *aPos + (a->size / 2.0f), aRot, &aRotInv);
			overlap.norm = -overlap.norm;
		}
		else if (b->kind == BODY_AABB || b->kind == BODY_OBB) {
			overlap = obbObbOverlap(*aPos, aVerts, *bPos, bVerts);
		}
	}
	
	if (!overlap.exists) { return; }
	
	if (a->isStatic) {
		*bPos -= overlap.norm * overlap.depth;
		//b->vel = glm::vec3(0.0f);
		b->vel *= -1;
	}
	else if (b->isStatic) {
		*aPos += overlap.norm * overlap.depth;
		//a->vel = glm::vec3(0.0f);
		a->vel *= -1;
	}
	else {
		*aPos += overlap.norm * (overlap.depth / 2.0f);
		*bPos -= overlap.norm * (overlap.depth / 2.0f);
		//a->vel = glm::vec3(0.0f);
		//b->vel = glm::vec3(0.0f);
		a->vel *= -1.0f;
		b->vel *= -1.0f;
	}
}
