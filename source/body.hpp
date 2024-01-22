#pragma once

#include <iostream>

#include <glm/gtx/orthonormalize.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

#include "geom.hpp"
#include "draw3d.hpp"

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
	float restitution;
	
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
	float sFriction, float dFriction,
	float restitution
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
		.restitution = restitution,
		
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
	float sFriction, float dFriction,
	float restitution
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
		.restitution = restitution,
		
		.vel = glm::vec3(0.0f),
		
		.rotVel = glm::vec3(0.0f),
		
		.force = glm::vec3(0.0f),
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
		.restitution = restitution,
		
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
	float sFriction, float dFriction,
	float restitution
) {
	return Body{
		.kind = BODY_SPHERE,
		.radius = radius,
		
		.isStatic = true,
		
		.volume = (4.0f * (float)M_PI * (radius * radius * radius)) / 3.0f,
		.invMass = 0.0f,
		.sFriction = sFriction,
		.dFriction = dFriction,
		.restitution = restitution
	};
}

static Body bodyCreateStaticAabb(
	glm::vec3 size,
	float sFriction, float dFriction,
	float restitution
) {
	return Body{
		.kind = BODY_AABB,
		.size = size,
		
		.isStatic = true,
		
		.volume = size.x * size.y * size.z,
		.invMass = 0.0f,
		.sFriction = sFriction,
		.dFriction = dFriction,
		.restitution = restitution
	};
}

static Body bodyCreateStaticObb(
	glm::vec3 size,
	float sFriction, float dFriction,
	float restitution
) {
	return Body{
		.kind = BODY_OBB,
		.size = size,
		
		.isStatic = true,
		
		.volume = size.x * size.y * size.z,
		.invMass = 0.0f,
		.sFriction = sFriction,
		.dFriction = dFriction,
		.restitution = restitution
	};
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
		
		body->rotVel = glm::inverse(body->inertiaTensor) * body->rotMom;
		//std::cout << body->rotVel.x << ", " << body->rotVel.y << ", " << body->rotVel.z << '\n';
	}
	
	body->force = glm::vec3(0.0f);
	body->torque = glm::vec3(0.0f);
}

static void bodyCollide(
	glm::vec3 *aPos, glm::mat3 *aRot, Body *a,
	glm::vec3 *bPos, glm::mat3 *bRot, Body *b
) {
	if (a->isStatic && b->isStatic) { return; }
	
	glm::mat3 aRotInv = glm::inverse(*aRot);
	glm::mat3 bRotInv = glm::inverse(*bRot);
	
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
			overlap = obbObbOverlap(*aPos, aVerts, *bPos, bVerts);
		}
	}
	else if (a->kind == BODY_OBB) {
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
	}
	else if (b->isStatic) {
		*aPos += overlap.norm * overlap.depth;
	}
	else {
		*aPos += overlap.norm * (overlap.depth / 2.0f);
		*bPos -= overlap.norm * (overlap.depth / 2.0f);
	}
	
	glm::vec3 contacts[4];
	size_t numContacts;
	if (a->kind == BODY_SPHERE) {
		if (b->kind == BODY_SPHERE) {
			contacts[0] = sphereSphereContact(*aPos, a->radius, *bPos, b->radius);
		}
		else if (b->kind == BODY_AABB) {
			contacts[0] = sphereAabbContact(*aPos, a->radius, *bPos - (b->size / 2.0f), *bPos + (b->size / 2.0f));
		}
		else if (b->kind == BODY_OBB) {
			contacts[0] = sphereObbContact(*aPos, a->radius, *bPos, *bPos - (b->size / 2.0f), *bPos + (b->size / 2.0f), bRot, &bRotInv);
		}
		numContacts = 1;
	}
	else if (a->kind == BODY_AABB) {
		if (b->kind == BODY_SPHERE) {
			contacts[0] = sphereAabbContact(*bPos, b->radius, *aPos - (a->size / 2.0f), *aPos + (a->size / 2.0f));
			numContacts = 1;
		}
		else if (b->kind == BODY_AABB) {
			aabbAabbContacts(*aPos - (a->size / 2.0f), *aPos + (a->size / 2.0f), *bPos - (b->size / 2.0f), *bPos + (b->size / 2.0f), contacts);
			numContacts = 4;
		}
		else if (b->kind == BODY_OBB) {
			numContacts = obbObbContacts(
				*aPos, *aPos - (a->size / 2.0f), *aPos + (a->size / 2.0f), &aRotInv, aVerts,
				*bPos, *bPos - (b->size / 2.0f), *bPos + (b->size / 2.0f), &bRotInv, bVerts,
				contacts
			);
		}
	}
	else if (a->kind == BODY_OBB) {
		if (b->kind == BODY_SPHERE) {
			contacts[0] = sphereObbContact(*bPos, b->radius, *aPos, *aPos - (a->size / 2.0f), *aPos + (a->size / 2.0f), aRot, &aRotInv);
			numContacts = 1;
		}
		else if (b->kind == BODY_AABB || b->kind == BODY_OBB) {
			numContacts = obbObbContacts(
				*aPos, *aPos - (a->size / 2.0f), *aPos + (a->size / 2.0f), &aRotInv, aVerts,
				*bPos, *bPos - (b->size / 2.0f), *bPos + (b->size / 2.0f), &bRotInv, bVerts,
				contacts
			);
		}
	}
	
	glm::vec3 norm = overlap.norm;
	float e = glm::min(a->restitution, b->restitution);
	
	glm::mat3 aInertiaTensorInv = (a->isStatic || a->kind == BODY_AABB)? glm::mat3(0.0f) : glm::inverse(a->inertiaTensor);
	glm::mat3 bInertiaTensorInv = (b->isStatic || b->kind == BODY_AABB)? glm::mat3(0.0f) : glm::inverse(b->inertiaTensor);
	
	float impulseMags[4];
	glm::vec3 impulses[4];
		
	for (size_t i = 0; i < numContacts; i++) {
		glm::vec3 contact = contacts[i];
		
		//queueDrawDebugPoint(contact, glm::vec3(1.0f, 1.0f, 0.0f), true, 0.5f);
		
		glm::vec3 velA = a->isStatic? glm::vec3(0.0f) : a->vel;
		glm::vec3 velB = b->isStatic? glm::vec3(0.0f) : b->vel;
		glm::vec3 rotVelA = (a->isStatic || a->kind == BODY_AABB)? glm::vec3(0.0f) : a->rotVel;
		glm::vec3 rotVelB = (b->isStatic || b->kind == BODY_AABB)? glm::vec3(0.0f) : b->rotVel;
		glm::vec3 ap = contact - *aPos;
		glm::vec3 bp = contact - *bPos;
		velA += glm::cross(rotVelA, ap);
		velB += glm::cross(rotVelB, bp);
		glm::vec3 velAB = velA - velB;
		glm::vec3 velBA = velB - velA;
		
		float j =
			glm::dot(-(1.0f + e) * velAB, norm) / (
				glm::dot(norm, norm * (a->invMass + b->invMass)) +
				glm::dot(norm,
					glm::cross(aInertiaTensorInv * glm::cross(ap, norm), ap) +
					glm::cross(bInertiaTensorInv * glm::cross(bp, norm), bp)
				)
			);
		j /= (float)numContacts;
		
		impulses[i] = j * norm;
		impulseMags[i] = j;
	}
	
	for (size_t i = 0; i < numContacts; i++) {
		glm::vec3 contact = contacts[i];
		glm::vec3 impulse = impulses[i];
		
		//queueDrawDebugLine(contact, contact + impulse, glm::vec3(1.0f, 0.0f, 0.0f), true, 0.5f);
		//queueDrawDebugLine(contact, contact - impulse, glm::vec3(0.0f, 1.0f, 0.0f), true, 0.5f);
		
		if (!a->isStatic) {
			glm::vec3 ap = contact - *aPos;
			a->vel += impulse / a->mass;
			a->rotMom += glm::cross(ap, impulse);
			//a->rotVel += aInertiaTensorInv * glm::cross(ap, impulse);
		}
		
		if (!b->isStatic) {
			glm::vec3 bp = contact - *bPos;
			b->vel += -impulse / b->mass;
			b->rotMom += glm::cross(bp, -impulse);
			//b->rotVel += bInertiaTensorInv * glm::cross(bp, -impulse);
		}
	}
	
	a->rotVel = glm::inverse(a->inertiaTensor) * a->rotMom;
	b->rotVel = glm::inverse(b->inertiaTensor) * b->rotMom;
	
	/*float sf = a->sFriction + ((b->sFriction - a->sFriction) / 2.0f);
	float df = a->dFriction + ((b->dFriction - a->dFriction) / 2.0f);
	
	glm::vec3 frictionImpulses[4];
	
	for (size_t i = 0; i < numContacts; i++) {
		glm::vec3 contact = contacts[i];
		
		//queueDrawDebugPoint(contact, glm::vec3(1.0f, 1.0f, 0.0f), true, 0.5f);
		
		glm::vec3 velA = a->isStatic? glm::vec3(0.0f) : a->vel;
		glm::vec3 velB = b->isStatic? glm::vec3(0.0f) : b->vel;
		glm::vec3 rotVelA = a->isStatic? glm::vec3(0.0f) : a->rotVel;
		glm::vec3 rotVelB = b->isStatic? glm::vec3(0.0f) : b->rotVel;
		glm::vec3 ap = contact - *aPos;
		glm::vec3 bp = contact - *bPos;
		velA += glm::cross(rotVelA, ap);
		velB += glm::cross(rotVelB, bp);
		glm::vec3 velAB = velA - velB;
		glm::vec3 velBA = velB - velA;
		
		glm::vec3 tan = velBA - (overlap.norm * (glm::dot(velBA, norm)));
		
		glm::mat3 aInertiaTensorInv = a->isStatic? glm::mat3(0.0f) : glm::inverse(a->inertiaTensor);
		glm::mat3 bInertiaTensorInv = b->isStatic? glm::mat3(0.0f) : glm::inverse(b->inertiaTensor);
		
		float jT =
			glm::dot(-velAB, tan) / (
				glm::dot(tan, tan * (a->invMass + b->invMass)) +
				glm::dot(tan,
					glm::cross(aInertiaTensorInv * glm::cross(ap, tan), ap) +
					glm::cross(bInertiaTensorInv * glm::cross(bp, tan), bp)
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
			a->rotMom += glm::inverse(a->inertiaTensor) * glm::cross(ap, frictionImpulse);
		}
		
		if (!b->isStatic) {
			glm::vec3 bp = contact - *bPos;
			b->vel -= -frictionImpulse / b->mass;
			b->rotMom -= glm::inverse(a->inertiaTensor) * glm::cross(bp, -frictionImpulse);
		}
	}*/
	
	//std::cout << "A: " << a->rotVel.x << ", " << a->rotVel.y << ", " << a->rotVel.z << '\n';
	//std::cout << "B: " << b->rotVel.x << ", " << b->rotVel.y << ", " << b->rotVel.z << '\n';
}
