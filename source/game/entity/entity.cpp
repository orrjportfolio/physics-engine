#include "entity.hpp"

#include <cassert>

#include <glm/gtx/orthonormalize.hpp>

#include "../gfx/scene3d.hpp"
#include "../geom.hpp"
#include "octree.hpp"

void Entity::simulateAll(float dt) {
	for (uint32_t i = 0; i < num; i++) {
		uint32_t k = flags[i].colliderKind;
		if (k == COLLIDER_KIND_KINEMATIC || k == COLLIDER_KIND_DYNAMIC) {
			if (vels[i] != glm::vec3(0.0f)) {
				glm::vec3 prevMin, prevMax;
				byIdx(i).colliderBounds(&prevMin, &prevMax);
				
				poses[i] += vels[i] * dt;
				
				glm::vec3 min, max;
				byIdx(i).colliderBounds(&min, &max);
				
				Octree::root.moveEntry(i, prevMin, prevMax, min, max);
			}
			
			if (k == COLLIDER_KIND_DYNAMIC) {
				auto accel = (forces[i] * invMasses[i]) + glm::vec3(0.0f, -9.8f, 0.0f);
				vels[i] += accel * dt;
			}
			
			forces[i] = glm::vec3(0.0f);
		}
	}
	
	for (uint32_t i = 0; i < num; i++) {
		uint32_t k = flags[i].colliderKind;
		if (
			(k == COLLIDER_KIND_KINEMATIC || k == COLLIDER_KIND_DYNAMIC) &&
			!flags[i].colliderIsAxisAligned
		) {
			if (rotVels[i] != glm::vec3(0.0f)) {
				auto rotVelMat = glm::mat3(
					0, -rotVels[i].z, rotVels[i].y,
					rotVels[i].z, 0, -rotVels[i].x,
					-rotVels[i].y, rotVels[i].x, 0
				);
				rots[i] = glm::orthonormalize(rots[i] + ((-rotVelMat * dt) * rots[i]));
			}
			
			if (k == COLLIDER_KIND_DYNAMIC) {
				invInertiaTensors[i] =
					rots[i] *
					(glm::mat3)glm::scale(invLocalInertiaTensors[i]) *
					glm::transpose(rots[i]);
				
				rotMoms[i] += torques[i] * dt;
				
				rotVels[i] = invInertiaTensors[i] * rotMoms[i];
			}
			
			torques[i] = glm::vec3(0.0f);
		}
	}
	
	for (uint32_t a = 0; a < num; a++) {
		uint32_t aK = flags[a].colliderKind;
		if (aK == COLLIDER_KIND_NONE) { continue; }
		
		auto aE = Entity::byIdx(a);
		
		glm::vec3 aMin, aMax;
		aE.colliderBounds(&aMin, &aMax);
		
		std::vector<uint32_t> overlaps;
		Octree::root.overlaps(aMin, aMax, overlaps);
		
		for (uint32_t b : overlaps) {
			if (a == b) { continue; }
		//for (uint32_t b = a + 1; b < num; b++) {
			uint32_t bK = flags[b].colliderKind;
			if (bK == COLLIDER_KIND_NONE || (
				(aK == COLLIDER_KIND_TRIGGER || aK == COLLIDER_KIND_KINEMATIC) &&
				(aK == bK)
			)) { continue; }
			
			
			auto bE = Entity::byIdx(b);
			
			glm::vec3 bMin, bMax;
			bE.colliderBounds(&bMin, &bMax);
			
			if (
				aMin.x > bMax.x || aMin.y > bMax.y || aMin.z > bMax.z ||
				bMin.x > aMax.x || bMin.y > aMax.y || bMin.z > aMax.z
			) { continue; }
			
			auto aSK = flags[a].colliderShapeKind;
			auto aAA = flags[a].colliderIsAxisAligned;
			auto &aPos = poses[a];
			auto aHS = colliderHalfSizes[a];
			
			auto bSK = flags[b].colliderShapeKind;
			auto bAA = flags[b].colliderIsAxisAligned;
			auto &bPos = poses[b];
			auto bHS = colliderHalfSizes[b];
			
			glm::vec3 aVerts[8], bVerts[8];
			
			Overlap overlap;
			if (aSK == ColliderShape::KIND_SPHERE) {
				if (bSK == ColliderShape::KIND_SPHERE) {
					overlap = Sphere::sphereOverlap(
						aPos, aHS.x,
						bPos, bHS.x
					);
				}
				else if (bAA) {
					overlap = Sphere::aabbOverlap(
						aPos, aHS.x,
						bPos - bHS, bPos + bHS
					);
				}
				else {
					overlap = Sphere::obbOverlap(
						aPos, aHS.x,
						bPos - bHS, bPos + bHS, bE.bodyMat(), bE.bodyMatInv()
					);
				}
			}
			else if (aAA) {
				if (bSK == ColliderShape::KIND_SPHERE) {
					overlap = Aabb::sphereOverlap(
						aPos - aHS, aPos + aHS,
						bPos, bHS.x
					);
				}
				else if (bAA) {
					overlap = Aabb::aabbOverlap(
						aPos - aHS, aPos + aHS,
						bPos - bHS, bPos + bHS
					);
				}
				else {
					Aabb::verts(aPos - aHS, aPos + aHS, aVerts);
					Obb::verts(bPos, bHS, rots[b], bVerts);
					
					overlap = Obb::obbOverlap(
						aPos, aVerts,
						bPos, bVerts
					);
				}
			}
			else {
				if (bSK == ColliderShape::KIND_SPHERE) {
					overlap = Obb::sphereOverlap(
						aPos - aHS, aPos + aHS, aE.bodyMat(), aE.bodyMatInv(),
						bPos, bHS.x
					);
				}
				else {
					Obb::verts(aPos, aHS, rots[a], aVerts);
					if (bAA) {
						Aabb::verts(bPos - bHS, bPos + bHS, bVerts);
					}
					else {
						Obb::verts(bPos, bHS, rots[b], bVerts);
					}
					
					overlap = Obb::obbOverlap(
						aPos, aVerts,
						bPos, bVerts
					);
				}
			}
			
			if (!overlap.exists) { continue; }
			
			auto correct = overlap.norm * overlap.depth;
			
			if (aK == COLLIDER_KIND_TRIGGER) {
				// TODO
				continue;
			}
			else if (bK == COLLIDER_KIND_TRIGGER) {
				// TODO
				continue;
			}
			else if (aK == COLLIDER_KIND_KINEMATIC) {
				bPos -= correct;
				
				glm::vec3 bNewMin, bNewMax;
				byIdx(b).colliderBounds(&bNewMin, &bNewMax);
				
				Octree::root.moveEntry(b, bMin, bMax, bNewMin, bNewMax);
			}
			else if (bK == COLLIDER_KIND_KINEMATIC) {
				aPos += correct;
				
				glm::vec3 aNewMin, aNewMax;
				byIdx(a).colliderBounds(&aNewMin, &aNewMax);
				
				Octree::root.moveEntry(a, aMin, aMax, aNewMin, aNewMax);
			}
			else {
				bPos -= correct / 2.0f;
				aPos += correct / 2.0f;
				
				glm::vec3
					bNewMin, bNewMax,
					aNewMin, aNewMax;
				byIdx(b).colliderBounds(&bNewMin, &bNewMax);
				byIdx(a).colliderBounds(&aNewMin, &aNewMax);
				
				Octree::root.moveEntry(a, aMin, aMax, aNewMin, aNewMax);
				Octree::root.moveEntry(b, bMin, bMax, bNewMin, bNewMax);
			}
			
			size_t numContacts;
			glm::vec3 contacts[8];
			if (aSK == ColliderShape::KIND_SPHERE) {
				numContacts = 1;
				if (bSK == ColliderShape::KIND_SPHERE) {
					contacts[0] = Sphere::sphereContact(
						aPos, aHS.x,
						bPos, bHS.x
					);
				}
				else if (bAA) {
					contacts[0] = Sphere::aabbContact(
						aPos, aHS.x,
						bPos - bHS, bPos + bHS
					);
				}
				else {
					contacts[0] = Sphere::obbContact(
						aPos, aHS.x,
						bPos - bHS, bPos + bHS, bE.bodyMat(), bE.bodyMatInv()
					);
				}
			}
			else if (aAA) {
				if (bSK == ColliderShape::KIND_SPHERE) {
					numContacts = 1;
					contacts[0] = Aabb::sphereContact(
						aPos - aHS, aPos + aHS,
						bPos, bHS.x
					);
				}
				else if (bAA) {
					numContacts = 4;
					Aabb::aabbContacts(
						aPos - aHS, aPos + aHS,
						bPos - bHS, bPos + bHS,
						contacts
					);
				}
				else {
					numContacts = Obb::obbContacts(
						aPos - aHS, aPos + aHS, glm::identity<glm::mat4>(), aVerts,
						bPos - bHS, bPos + bHS, bE.bodyMat(), bVerts,
						contacts
					);
				}
			}
			else {
				if (bSK == ColliderShape::KIND_SPHERE) {
					numContacts = 1;
					contacts[0] = Obb::sphereContact(
						aPos - aHS, aPos + aHS, aE.bodyMat(), aE.bodyMatInv(),
						bPos, bHS.x
					);
				}
				else if (bAA) {
					numContacts = Obb::obbContacts(
						aPos - aHS, aPos + aHS, aE.bodyMat(), aVerts,
						bPos - bHS, bPos + bHS, glm::identity<glm::mat4>(), bVerts,
						contacts
					);
				}
				else {
					numContacts = Obb::obbContacts(
						aPos - aHS, aPos + aHS, aE.bodyMat(), aVerts,
						bPos - bHS, bPos + bHS, bE.bodyMat(), bVerts,
						contacts
					);
				}
			}
			
			/*for (size_t i = 0; i < numContacts; i++) {
				Scene3d::addDebugPoint(contacts[i], glm::vec3(1.0f), true, 0.5f);
			}*/
			
			auto n = overlap.norm;
			auto e = glm::max(bouncinesses[a], bouncinesses[b]);
			
			auto &aVel = vels[a];
			auto aInvMass = invMasses[a];
			auto &aRotMom = rotMoms[a];
			auto &aRotVel = rotVels[a];
			auto aInvInertiaTensor = invInertiaTensors[a];
			
			auto &bVel = vels[b];
			auto bInvMass = invMasses[b];
			auto &bRotMom = rotMoms[b];
			auto &bRotVel = rotVels[b];
			auto bInvInertiaTensor = invInertiaTensors[b];
			
			float impulseMags[8];
			
			for (size_t i = 0; i < numContacts; i++) {
				auto contact = contacts[i];
				
				auto ap = contact - aPos;
				auto bp = contact - bPos;
				auto apVel = aVel + glm::cross(aRotVel, ap);
				auto bpVel = bVel + glm::cross(bRotVel, bp);
				auto abVel = apVel - bpVel;
				auto baVel = bpVel - apVel;
				
				float j =
					glm::dot(-(1.0f + e) * abVel, n) / (
						glm::dot(n, n * (aInvMass + bInvMass)) +
						glm::dot(n,
							glm::cross(aInvInertiaTensor * glm::cross(ap, n), ap) +
							glm::cross(bInvInertiaTensor * glm::cross(bp, n), bp)
						)
					) / (float)numContacts;
				
				impulseMags[i] = j;
			}
			
			for (size_t i = 0; i < numContacts; i++) {
				auto contact = contacts[i];
				auto impulse = impulseMags[i] * n;
				
				//Scene3d::addDebugLine(contact, contact + impulse, glm::vec3(1.0f, 0.0f, 0.0f), true, 0.5f);
				//Scene3d::addDebugLine(contact, contact - impulse, glm::vec3(0.0f, 1.0f, 1.0f), true, 0.5f);
				
				if (aK == COLLIDER_KIND_DYNAMIC) {
					aVel += impulse * aInvMass;
					if (!aAA) {
						aRotMom += glm::cross(contact - aPos, impulse);
					}
				}
				
				if (bK == COLLIDER_KIND_DYNAMIC) {
					bVel += -impulse * bInvMass;
					if (!bAA) {
						bRotMom += glm::cross(contact - bPos, -impulse);
					}
				}
			}
			
			aRotVel = aInvInertiaTensor * aRotMom;
			bRotVel = bInvInertiaTensor * bRotMom;
			
			float sFrict = (sFricts[a] + sFricts[b]) / 2.0f;
			float dFrict = (dFricts[a] + dFricts[b]) / 2.0f;
			
			glm::vec3 frictImpulses[8];
			
			for (size_t i = 0; i < numContacts; i++) {
				auto contact = contacts[i];
				
				auto ap = contact - aPos;
				auto bp = contact - bPos;
				auto apVel = aVel + glm::cross(aRotVel, ap);
				auto bpVel = bVel + glm::cross(bRotVel, bp);
				auto abVel = apVel - bpVel;
				auto baVel = bpVel - apVel;
				
				auto tan = abVel - (n * glm::dot(abVel, n));
				if (length(tan) == 0.0f) {
					frictImpulses[i] = glm::vec3(0.0f);
					continue;
				}
				tan = glm::normalize(tan);
				
				float jT =
					glm::dot(-abVel, tan) / (
						glm::dot(tan, tan * (aInvMass + bInvMass)) +
						glm::dot(tan,
							glm::cross(aInvInertiaTensor * glm::cross(ap, tan), ap) +
							glm::cross(bInvInertiaTensor * glm::cross(bp, tan), bp)
						)
					) / (float)numContacts;
				
				float j = impulseMags[i];
				
				frictImpulses[i] =
					(glm::abs(jT) <= j * sFrict)?
						(jT * tan) :
						(-j * tan * dFrict);
			}
			
			for (size_t i = 0; i < numContacts; i++) {
				auto contact = contacts[i];
				auto frictionImpulse = frictImpulses[i];
				
				//Scene3d::addDebugLine(contact, contact + frictionImpulse, glm::vec3(1.0f, 1.0f, 0.0f), true, 0.5f);
				//Scene3d::addDebugLine(contact, contact - frictionImpulse, glm::vec3(0.0f, 0.0f, 1.0f), true, 0.5f);
				
				if (aK == COLLIDER_KIND_DYNAMIC) {
					aVel += frictionImpulse * aInvMass;
					if (!aAA) {
						aRotMom += glm::cross(contact - aPos, frictionImpulse);
					}
				}
				
				if (bK == COLLIDER_KIND_DYNAMIC) {
					bVel += -frictionImpulse * bInvMass;
					if (!bAA) {
						bRotMom += glm::cross(contact - bPos, -frictionImpulse);
					}
				}
			}
			
			aRotVel = aInvInertiaTensor * aRotMom;
			bRotVel = bInvInertiaTensor * bRotMom;
		}
	}
}

void Entity::addAllToScene3d() {
	for (uint32_t i = 0; i < num; i++) {
		if (flags[i].hasMesh) {
			Scene3d::addObject(
				meshes[i].mesh,
				meshes[i].material,
				(
					glm::translate(poses[i] + meshes[i].offs) *
					glm::mat4(rots[i]) *
					glm::scale(meshes[i].scale)
				)
			);
		}
	}
}

Entity Entity::create(glm::vec3 pos, glm::mat3 const &rot) {
	uint32_t idx;
	if (num < CAP) {
		idx = num++;
	}
	else {
		assert(numEmpties > 0);
		idx = empties[--numEmpties];
	}
	
	uint32_t gen = gens[idx];
	
	poses[idx] = pos;
	rots[idx] = rot;
	
	return Entity{
		.idx = idx,
		.gen = gen
	};
}

void Entity::destroy() {
	flags[idx].asInt = 0;
	gens[idx]++;
	
	empties[numEmpties++] = idx;
}

void Entity::setPos(glm::vec3 pos) {
	if (flags[idx].colliderKind != COLLIDER_KIND_NONE) {
		glm::vec3 prevMin, prevMax;
		colliderBounds(&prevMin, &prevMax);
		
		poses[idx] = pos;
		
		glm::vec3 min, max;
		colliderBounds(&min, &max);
		Octree::root.moveEntry(idx, prevMin, prevMax, min, max);
	}
	else {
		poses[idx] = pos;
	}
}

void Entity::makeTrigger(ColliderShape shape) {
	flags[idx].colliderKind = COLLIDER_KIND_TRIGGER;
	flags[idx].colliderShapeKind = shape.kind;
	flags[idx].colliderIsAxisAligned = shape.isAxisAligned;
	
	colliderHalfSizes[idx] = shape.halfSize;
	
	glm::vec3 min, max;
	colliderBounds(&min, &max);
	Octree::root.addEntry(idx, min, max);
}

void Entity::makeKinematic(ColliderShape shape, PhysicsMaterial material) {
	flags[idx].colliderKind = COLLIDER_KIND_KINEMATIC;
	flags[idx].colliderShapeKind = shape.kind;
	flags[idx].colliderIsAxisAligned = shape.isAxisAligned;
	
	colliderHalfSizes[idx] = shape.halfSize;
	
	sFricts[idx] = material.sFrict;
	dFricts[idx] = material.dFrict;
	bouncinesses[idx] = material.bounciness;
	
	vels[idx] = glm::vec3(0.0f);
	invMasses[idx] = 0.0f;
	forces[idx] = glm::vec3(0.0f);
	
	rotVels[idx] = glm::vec3(0.0f);
	invLocalInertiaTensors[idx] = glm::vec3(0.0f);
	invInertiaTensors[idx] = glm::mat3(0.0f);
	torques[idx] = glm::vec3(0.0f);
	
	glm::vec3 min, max;
	colliderBounds(&min, &max);
	Octree::root.addEntry(idx, min, max);
}

void Entity::makeDynamic(ColliderShape shape, PhysicsMaterial material, float density) {
	glm::vec3 size = shape.halfSize * 2.0f;
	
	float volume =
		(shape.kind == ColliderShape::KIND_SPHERE)?
			(4.0f * (float)M_PI * shape.halfSize.x * shape.halfSize.x * shape.halfSize.x) / 3.0f :
			size.x * size.y * size.z;
	float mass = density * volume;
	
	auto invLocalInertiaTensor =
		(shape.kind == ColliderShape::KIND_SPHERE)?
			glm::vec3(5.0f / (2.0f * mass * shape.halfSize.x * shape.halfSize.x)) :
			glm::vec3(
				12.0f / (mass * ((size.y * size.y) + (size.z * size.z))),
				12.0f / (mass * ((size.x * size.x) + (size.y * size.y))),
				12.0f / (mass * ((size.x * size.x) + (size.z * size.z)))
			);
	
	flags[idx].colliderKind = COLLIDER_KIND_DYNAMIC;
	flags[idx].colliderShapeKind = shape.kind;
	flags[idx].colliderIsAxisAligned = shape.isAxisAligned;
	
	colliderHalfSizes[idx] = shape.halfSize;
	
	sFricts[idx] = material.sFrict;
	dFricts[idx] = material.dFrict;
	bouncinesses[idx] = material.bounciness;
	
	vels[idx] = glm::vec3(0.0f);
	invMasses[idx] = 1.0f / mass;
	forces[idx] = glm::vec3(0.0f);
	
	rotMoms[idx] = glm::vec3(0.0f);
	rotVels[idx] = glm::vec3(0.0f);
	invLocalInertiaTensors[idx] = invLocalInertiaTensor;
	invInertiaTensors[idx] = rots[idx] * glm::mat3(
		invLocalInertiaTensor.x, 0, 0,
		0, invLocalInertiaTensor.y, 0,
		0, 0, invLocalInertiaTensor.z
	) * glm::transpose(rots[idx]);
	torques[idx] = glm::vec3(0.0f);
	
	glm::vec3 min, max;
	colliderBounds(&min, &max);
	Octree::root.addEntry(idx, min, max);
}
