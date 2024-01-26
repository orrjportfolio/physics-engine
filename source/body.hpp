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
		glm::vec3 extents;
	};
	
	bool isStatic;
	
	float volume;
	float mass, massInv;
	glm::mat3 initInertiaTensor;
	float sFriction, dFriction;
	float restitution;
	
	glm::vec3 vel;
	glm::vec3 rotMom;
	
	glm::mat3 inertiaTensor, inertiaTensorInv;
	glm::vec3 rotVel;
	
	glm::vec3 force, torque;
};

static Body bodyCreateSphere(
	glm::mat3 rot,
	float radius,
	float density,
	float sFriction, float dFriction,
	float restitution
) {
	float volume = (4.0f * (float)M_PI * (radius * radius * radius)) / 3.0f;
	float mass = density * volume;
	glm::mat3 initInertiaTensor = glm::mat3((2.0f * mass * (radius * radius)) / 5.0f);
	glm::mat3 inertiaTensor = rot * initInertiaTensor * glm::transpose(rot);
	
	return Body{
		.kind = BODY_SPHERE,
		.radius = radius,
		
		.isStatic = false,
		
		.volume = volume,
		.mass = mass,
		.massInv = 1.0f / mass,
		.initInertiaTensor = initInertiaTensor,
		.sFriction = sFriction,
		.dFriction = dFriction,
		.restitution = restitution,
		
		.vel = glm::vec3(0.0f),
		.rotMom = glm::vec3(0.0f),
		
		.inertiaTensor = inertiaTensor,
		.inertiaTensorInv = glm::inverse(inertiaTensor),
		.rotVel = glm::vec3(0.0f),
		
		.force = glm::vec3(0.0f),
		.torque = glm::vec3(0.0f)
	};
}

static Body bodyCreateAabb(
	glm::vec3 size,
	float density,
	float sFriction, float dFriction,
	float restitution
) {
	float volume = size.x * size.y * size.z;
	float mass = density * volume;
	
	return Body{
		.kind = BODY_AABB,
		.extents = size / 2.0f,
		
		.isStatic = false,
		
		.volume = volume,
		.mass = mass,
		.massInv = 1.0f / mass,
		.initInertiaTensor = glm::mat3(0.0f),
		.sFriction = sFriction,
		.dFriction = dFriction,
		.restitution = restitution,
		
		.vel = glm::vec3(0.0f),
		.rotMom = glm::vec3(0.0f),
		
		.inertiaTensor = glm::mat3(0.0f),
		.inertiaTensorInv = glm::mat3(0.0f),
		.rotVel = glm::vec3(0.0f),
		
		.force = glm::vec3(0.0f),
		.torque = glm::vec3(0.0f),
	};
}

static Body bodyCreateObb(
	glm::mat3 rot,
	glm::vec3 size,
	float density,
	float sFriction, float dFriction,
	float restitution
) {
	float volume = size.x * size.y * size.z;
	float mass = density * volume;
	glm::mat3 initInertiaTensor = glm::mat3(
		glm::vec3((mass * ((size.y * size.y) + (size.z * size.z))) / 12.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, (mass * ((size.x * size.x) + (size.y * size.y))) / 12.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, (mass * ((size.x * size.x) + (size.z * size.z))) / 12.0f)
	);
	glm::mat3 inertiaTensor = rot * initInertiaTensor * glm::transpose(rot);
	
	return Body{
		.kind = BODY_OBB,
		.extents = size / 2.0f,
		
		.isStatic = false,
		
		.volume = volume,
		.mass = mass,
		.massInv = 1.0f / mass,
		.initInertiaTensor = initInertiaTensor,
		.sFriction = sFriction,
		.dFriction = dFriction,
		.restitution = restitution,
		
		.vel = glm::vec3(0.0f),
		.rotMom = glm::vec3(0.0f),
		
		.inertiaTensor = inertiaTensor,
		.inertiaTensorInv = glm::inverse(inertiaTensor),
		.rotVel = glm::vec3(0.0f),
		
		.force = glm::vec3(0.0f),
		.torque = glm::vec3(0.0f)
	};
}

static Body bodyCreateStaticSphere(
	glm::mat3 rot,
	float radius,
	float sFriction, float dFriction,
	float restitution
) {
	return Body{
		.kind = BODY_SPHERE,
		.radius = radius,
		
		.isStatic = true,
		
		.volume = (4.0f * (float)M_PI * (radius * radius * radius)) / 3.0f,
		.massInv = 0.0f,
		.initInertiaTensor = glm::mat3(0.0f),
		.sFriction = sFriction,
		.dFriction = dFriction,
		.restitution = restitution,
		
		.vel = glm::vec3(0.0f),
		.rotMom = glm::vec3(0.0f),
		
		.inertiaTensor = glm::mat3(0.0f),
		.inertiaTensorInv = glm::mat3(0.0f),
		.rotVel = glm::vec3(0.0f)
	};
}

static Body bodyCreateStaticAabb(
	glm::vec3 size,
	float sFriction, float dFriction,
	float restitution
) {
	return Body{
		.kind = BODY_AABB,
		.extents = size / 2.0f,
		
		.isStatic = true,
		
		.volume = size.x * size.y * size.z,
		.massInv = 0.0f,
		.sFriction = sFriction,
		.dFriction = dFriction,
		.restitution = restitution,
		
		.vel = glm::vec3(0.0f),
		.rotMom = glm::vec3(0.0f),
		
		.inertiaTensor = glm::mat3(0.0f),
		.inertiaTensorInv = glm::mat3(0.0f),
		.rotVel = glm::vec3(0.0f)
	};
}

static Body bodyCreateStaticObb(
	glm::vec3 size,
	float sFriction, float dFriction,
	float restitution
) {
	return Body{
		.kind = BODY_OBB,
		.extents = size / 2.0f,
		
		.isStatic = true,
		
		.volume = size.x * size.y * size.z,
		.massInv = 0.0f,
		.sFriction = sFriction,
		.dFriction = dFriction,
		.restitution = restitution,
		
		.vel = glm::vec3(0.0f),
		.rotMom = glm::vec3(0.0f),
		
		.inertiaTensor = glm::mat3(0.0f),
		.inertiaTensorInv = glm::mat3(0.0f),
		.rotVel = glm::vec3(0.0f)
	};
}

static void bodyBounds(glm::vec3 bodyPos, Body *body, glm::vec3 *oBoundsMin, glm::vec3 *oBoundsMax) {
	if (body->kind == BODY_SPHERE) {
		*oBoundsMin = bodyPos - body->radius;
		*oBoundsMax = bodyPos + body->radius;
	}
	else if (body->kind == BODY_AABB) {
		*oBoundsMin = bodyPos - body->extents;
		*oBoundsMax = bodyPos + body->extents;
	}
	else if (body->kind == BODY_OBB) {
		float radius = std::max({body->extents.x, body->extents.y, body->extents.z}) * (float)M_SQRT2;
		
		*oBoundsMin = bodyPos - radius;
		*oBoundsMax = bodyPos + radius;
	}
}

static void bodySimulate(glm::vec3 *bodyPos, glm::mat3 *bodyRot, Body *body, glm::vec3 gravity, float dt) {
	if (body->isStatic) { return; }
	
	*bodyPos += body->vel * dt;
	
	glm::vec3 accel = (body->force / body->mass) + gravity;
	body->vel += accel * dt;
	
	if (body->kind != BODY_AABB) {
		glm::mat3 velMat = glm::mat3(
			glm::vec3(0.0f, body->rotVel.z, -body->rotVel.y),
			glm::vec3(-body->rotVel.z, 0.0f, body->rotVel.x),
			glm::vec3(body->rotVel.y, -body->rotVel.x, 0.0f)
		);
		*bodyRot = *bodyRot + ((velMat * dt) * *bodyRot);
		*bodyRot = glm::orthonormalize(*bodyRot);
		
		body->rotMom += body->torque * dt;
		
		body->inertiaTensor = *bodyRot * body->initInertiaTensor * glm::transpose(*bodyRot);
		body->inertiaTensorInv = glm::inverse(body->inertiaTensor);
		
		body->rotVel = body->inertiaTensorInv * body->rotMom;
	}
	
	body->force = glm::vec3(0.0f);
	body->torque = glm::vec3(0.0f);
}

struct BodyCollision {
	bool occurred;
	size_t numContacts;
	glm::vec3 contacts[8];
	glm::vec3 aVel;
	glm::vec3 bVel;
};

static BodyCollision bodyCollide(
	glm::vec3 *aPos, glm::mat3 *aRot, Body *a,
	glm::vec3 *bPos, glm::mat3 *bRot, Body *b
) {
	if (a->isStatic && b->isStatic) {
		return BodyCollision{.occurred = false};
	}
	
	glm::vec3
		aBoundsMin, aBoundsMax,
		bBoundsMin, bBoundsMax;
	
	bodyBounds(*aPos, a, &aBoundsMin, &aBoundsMax);
	bodyBounds(*bPos, b, &bBoundsMin, &bBoundsMax);
	
	if (
		aBoundsMin.x > bBoundsMax.x || bBoundsMin.x > aBoundsMax.x ||
		aBoundsMin.y > bBoundsMax.y || bBoundsMin.y > aBoundsMax.y ||
		aBoundsMin.z > bBoundsMax.z || bBoundsMin.z > aBoundsMax.z
	) {
		return BodyCollision{.occurred = false};
	}
	
	glm::mat3 aRotInv = glm::inverse(*aRot);
	glm::mat3 bRotInv = glm::inverse(*bRot);
	
	glm::vec3 aVerts[8], bVerts[8];
	
	if (a->kind == BODY_AABB) {
		aabbVerts(*aPos + a->extents, *aPos - a->extents, aVerts);
	}
	else if (a->kind == BODY_OBB) {
		obbVerts(*aPos, a->extents, *aRot, aVerts);
	}
	
	if (b->kind == BODY_AABB) {
		aabbVerts(*bPos + b->extents, *bPos - b->extents, bVerts);
	}
	else if (b->kind == BODY_OBB) {
		obbVerts(*bPos, b->extents, *bRot, bVerts);
	}
	
	Overlap overlap;
	if (a->kind == BODY_SPHERE) {
		if (b->kind == BODY_SPHERE) {
			overlap = sphereSphereOverlap(*aPos, a->radius, *bPos, b->radius);
		}
		else if (b->kind == BODY_AABB) {
			overlap = sphereAabbOverlap(*aPos, a->radius, *bPos - b->extents, *bPos + b->extents);
		}
		else if (b->kind == BODY_OBB) {
			overlap = sphereObbOverlap(*aPos, a->radius, *bPos, *bPos - b->extents, *bPos + b->extents, bRot, &bRotInv);
		}
	}
	else if (a->kind == BODY_AABB) {
		if (b->kind == BODY_SPHERE) {
			overlap = sphereAabbOverlap(*bPos, b->radius, *aPos - a->extents, *aPos + a->extents);
			overlap.norm = -overlap.norm;
		}
		else if (b->kind == BODY_AABB) {
			overlap = aabbAabbOverlap(*aPos - a->extents, *aPos + a->extents, *bPos - b->extents, *bPos + b->extents);
		}
		else if (b->kind == BODY_OBB) {
			overlap = obbObbOverlap(*aPos, aVerts, *bPos, bVerts);
		}
	}
	else if (a->kind == BODY_OBB) {
		if (b->kind == BODY_SPHERE) {
			overlap = sphereObbOverlap(*bPos, b->radius, *aPos, *aPos - a->extents, *aPos + a->extents, aRot, &aRotInv);
			overlap.norm = -overlap.norm;
		}
		else if (b->kind == BODY_AABB || b->kind == BODY_OBB) {
			overlap = obbObbOverlap(*aPos, aVerts, *bPos, bVerts);
		}
	}
	
	if (!overlap.exists) {
		return BodyCollision{.occurred = false};
	}
	
	if (a->isStatic) {
		*bPos -= overlap.norm * overlap.depth;
	}
	else if (b->isStatic) {
		*aPos += overlap.norm * overlap.depth;
	}
	else {
		*aPos += overlap.norm * (overlap.depth / 2.0f);
		*bPos -= overlap.norm * (overlap.depth / 2.0f);
	}
	
	glm::vec3 contacts[8];
	size_t numContacts;
	if (a->kind == BODY_SPHERE) {
		if (b->kind == BODY_SPHERE) {
			contacts[0] = sphereSphereContact(*aPos, a->radius, *bPos, b->radius);
		}
		else if (b->kind == BODY_AABB) {
			contacts[0] = sphereAabbContact(*aPos, a->radius, *bPos - b->extents, *bPos + b->extents);
		}
		else if (b->kind == BODY_OBB) {
			contacts[0] = sphereObbContact(*aPos, a->radius, *bPos, *bPos - b->extents, *bPos + b->extents, bRot, &bRotInv);
		}
		numContacts = 1;
	}
	else if (a->kind == BODY_AABB) {
		if (b->kind == BODY_SPHERE) {
			contacts[0] = sphereAabbContact(*bPos, b->radius, *aPos - a->extents, *aPos + a->extents);
			numContacts = 1;
		}
		else if (b->kind == BODY_AABB) {
			aabbAabbContacts(*aPos - a->extents, *aPos + a->extents, *bPos - b->extents, *bPos + b->extents, contacts);
			numContacts = 4;
		}
		else if (b->kind == BODY_OBB) {
			numContacts = obbObbContacts(
				*aPos, *aPos - a->extents, *aPos + a->extents, &aRotInv, aVerts,
				*bPos, *bPos - b->extents, *bPos + b->extents, &bRotInv, bVerts,
				contacts
			);
		}
	}
	else if (a->kind == BODY_OBB) {
		if (b->kind == BODY_SPHERE) {
			contacts[0] = sphereObbContact(*bPos, b->radius, *aPos, *aPos - a->extents, *aPos + a->extents, aRot, &aRotInv);
			numContacts = 1;
		}
		else if (b->kind == BODY_AABB || b->kind == BODY_OBB) {
			numContacts = obbObbContacts(
				*aPos, *aPos - a->extents, *aPos + a->extents, &aRotInv, aVerts,
				*bPos, *bPos - b->extents, *bPos + b->extents, &bRotInv, bVerts,
				contacts
			);
		}
	}
	
	glm::vec3 norm = overlap.norm;
	float e = glm::max(a->restitution, b->restitution);
	
	float impulseMags[8];
	glm::vec3 impulses[8];
		
	for (size_t i = 0; i < numContacts; i++) {
		glm::vec3 contact = contacts[i];
		
		glm::vec3 velA = a->vel;
		glm::vec3 velB = b->vel;
		glm::vec3 rotVelA = a->rotVel;
		glm::vec3 rotVelB = b->rotVel;
		glm::vec3 ap = contact - *aPos;
		glm::vec3 bp = contact - *bPos;
		velA += glm::cross(rotVelA, ap);
		velB += glm::cross(rotVelB, bp);
		glm::vec3 velAB = velA - velB;
		glm::vec3 velBA = velB - velA;
		
		float j =
			glm::dot(-(1.0f + e) * velAB, norm) / (
				glm::dot(norm, norm * (a->massInv + b->massInv)) +
				glm::dot(norm,
					glm::cross(a->inertiaTensorInv * glm::cross(ap, norm), ap) +
					glm::cross(b->inertiaTensorInv * glm::cross(bp, norm), bp)
				)
			);
		j /= (float)numContacts;
		
		impulses[i] = j * norm;
		impulseMags[i] = j;
	}
	
	for (size_t i = 0; i < numContacts; i++) {
		glm::vec3 contact = contacts[i];
		glm::vec3 impulse = impulses[i];
		
		if (!a->isStatic) {
			glm::vec3 ap = contact - *aPos;
			a->vel += impulse / a->mass;
			a->rotMom += glm::cross(ap, impulse);
		}
		
		if (!b->isStatic) {
			glm::vec3 bp = contact - *bPos;
			b->vel += -impulse / b->mass;
			b->rotMom += glm::cross(bp, -impulse);
		}
	}
	
	a->rotVel = a->inertiaTensorInv * a->rotMom;
	b->rotVel = b->inertiaTensorInv * b->rotMom;
	
	float sf = a->sFriction + ((b->sFriction - a->sFriction) / 2.0f);
	float df = a->dFriction + ((b->dFriction - a->dFriction) / 2.0f);
	
	glm::vec3 frictionImpulses[8];
	
	for (size_t i = 0; i < numContacts; i++) {
		glm::vec3 contact = contacts[i];
		
		glm::vec3 velA = a->vel;
		glm::vec3 velB = b->vel;
		glm::vec3 rotVelA = a->rotVel;
		glm::vec3 rotVelB = b->rotVel;
		glm::vec3 ap = contact - *aPos;
		glm::vec3 bp = contact - *bPos;
		velA += glm::cross(rotVelA, ap);
		velB += glm::cross(rotVelB, bp);
		glm::vec3 velAB = velA - velB;
		glm::vec3 velBA = velB - velA;
		
		glm::vec3 tan = velAB - (norm * (glm::dot(velAB, norm)));
		
		if (glm::length(tan) < 0.01f) {
			frictionImpulses[i] = glm::vec3(0.0f);
		}
		
		float jT =
			glm::dot(-velAB, tan) / (
				glm::dot(tan, tan * (a->massInv + b->massInv)) +
				glm::dot(tan,
					glm::cross(a->inertiaTensorInv * glm::cross(ap, tan), ap) +
					glm::cross(b->inertiaTensorInv * glm::cross(bp, tan), bp)
				)
			);
		jT /= (float)numContacts;
		float j = impulseMags[i];
		
		if (glm::abs(jT) < j * sf) {
			frictionImpulses[i] = jT * tan;
		}
		else {
			frictionImpulses[i] = -j * tan * df;
		}
	}
	
	for (size_t i = 0; i < numContacts; i++) {
		glm::vec3 contact = contacts[i];
		glm::vec3 frictionImpulse = frictionImpulses[i];
		
		if (!a->isStatic) {
			glm::vec3 ap = contact - *aPos;
			a->vel += frictionImpulse / a->mass;
			a->rotMom += glm::cross(ap, frictionImpulse);
		}
		
		if (!b->isStatic) {
			glm::vec3 bp = contact - *bPos;
			b->vel += -frictionImpulse / b->mass;
			b->rotMom += glm::cross(bp, -frictionImpulse);
		}
	}
	
	a->rotVel = a->inertiaTensorInv * a->rotMom;
	b->rotVel = b->inertiaTensorInv * b->rotMom;
	
	BodyCollision collision = BodyCollision{
		.occurred = true,
		.numContacts = numContacts,
		.aVel = a->vel,
		.bVel = b->vel
	};
	
	memcpy(collision.contacts, contacts, sizeof(contacts));
	
	return collision;
}
