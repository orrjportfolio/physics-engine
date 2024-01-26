#pragma once

#include <assert.h>
#include <stdint.h>

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

#include "body.hpp"
#include "draw3d.hpp"
#include "gfx.hpp"

enum : uint32_t {
	ENTITY_HAS_MESH = 0x1,
	ENTITY_HAS_BODY = 0x2
};

struct EntitySlot {
	bool isOccupied;
	uint8_t gen;
	uint32_t flags;
};

constexpr uint8_t CAP_ENTITY_SLOTS = 128;

static inline EntitySlot
	entitySlots[CAP_ENTITY_SLOTS];
static inline uint8_t
	numEntitySlots;

static inline uint8_t
	freeEntitySlots[CAP_ENTITY_SLOTS];
static inline uint8_t
	numFreeEntitySlots;

struct EntityId {
	uint8_t slot, gen;
};

struct EntityCallbacks {
	void (*onUpdate)(EntityId entity) =
		nullptr;
	void (*onCollide)(EntityId entity, glm::vec3 entityVel, uint8_t otherEntity, glm::vec3 otherEntityVel, size_t numContacts, glm::vec3 const *contacts) =
		nullptr;
};

static EntityCallbacks
	entityCallbacks[CAP_ENTITY_SLOTS];

static void updateEntities() {
	for (size_t i = 0; i < (size_t)numEntitySlots; i++) {
		if (entityCallbacks[i].onUpdate != nullptr) {
			entityCallbacks[i].onUpdate(EntityId{
				.slot = (uint8_t)i,
				.gen = entitySlots[i].gen
			});
		}
	}
}

struct EntityTransform {
	glm::vec3 pos;
	glm::mat3 rot;
};

static inline EntityTransform
	entityTransforms[CAP_ENTITY_SLOTS];

static inline Body
	entityBodies[CAP_ENTITY_SLOTS];

static void simulateEntityBodies(glm::vec3 gravity, float dt) {
	//std::vector<int8_t> physEntities;
	
	for (size_t i = 0; i < (size_t)numEntitySlots; i++) {
		if (
			!entitySlots[i].isOccupied ||
			!(entitySlots[i].flags & ENTITY_HAS_BODY)
		) { continue; }
		
		bodySimulate(
			&entityTransforms[i].pos,
			&entityTransforms[i].rot,
			&entityBodies[i],
			gravity,
			dt
		);
		
		//physEntities.push_back((uint8_t)i);
	}
	
	/*std::sort(physEntities.begin(), physEntities.end(), [&](uint8_t a, uint8_t b) {
		return entityTransforms[a].pos.x < entityTransforms[b].pos.x;
	});
	
	std::vector<int8_t> ai;
	float aiMin = INFINITY;
	float aiMax = -INFINITY;
	
	for (int8_t e : physEntities) {
		glm::vec3 eBoundsMin, eBoundsMax;
		bodyBounds(entityTransforms[e].pos, &entityBodies[e], &eBoundsMin, &eBoundsMax);
		
		float
			eXMin = eBoundsMin.x,
			eXMax = eBoundsMax.x;
		
		if (eXMin >= aiMax || aiMin >= eXMax) {
			aiMin = INFINITY;
			aiMax = -INFINITY;
			
			for (size_t i = 0; i < ai.size(); i++) {
				for (size_t j = i + 1; j < ai.size(); j++) {
					bodyCollide(
						&entityTransforms[ai[i]].pos, &entityTransforms[ai[i]].rot, &entityBodies[ai[i]],
						&entityTransforms[ai[j]].pos, &entityTransforms[ai[j]].rot, &entityBodies[ai[j]]
					);
				}
			}
			
			ai.clear();
		}
		
		aiMin = glm::min(aiMin, eXMin);
		aiMax = glm::max(aiMax, eXMax);
		
		ai.push_back(e);
	}
	
	for (size_t i = 0; i < ai.size(); i++) {
		for (size_t j = i + 1; j < ai.size(); j++) {
			bodyCollide(
				&entityTransforms[ai[i]].pos, &entityTransforms[ai[i]].rot, &entityBodies[ai[i]],
				&entityTransforms[ai[j]].pos, &entityTransforms[ai[j]].rot, &entityBodies[ai[j]]
			);
		}
	}*/
	
	for (size_t i = 0; i < (size_t)numEntitySlots; i++) {
		for (size_t j = i + 1; j < (size_t)numEntitySlots; j++) {
			BodyCollision collision = bodyCollide(
				&entityTransforms[i].pos, &entityTransforms[i].rot, &entityBodies[i],
				&entityTransforms[j].pos, &entityTransforms[j].rot, &entityBodies[j]
			);
			
			if (collision.occurred) {
				if (entityCallbacks[i].onCollide != nullptr) {
					entityCallbacks[i].onCollide(EntityId{
						.slot = (uint8_t)i,
						.gen = entitySlots[i].gen
					}, collision.aVel, (uint8_t)j, collision.bVel, collision.numContacts, collision.contacts);
				}
				
				if (entityCallbacks[j].onCollide != nullptr) {
					entityCallbacks[j].onCollide(EntityId{
						.slot = (uint8_t)j,
						.gen = entitySlots[j].gen
					}, collision.bVel, (uint8_t)i, collision.aVel, collision.numContacts, collision.contacts);
				}
			}
		}
	}
}

static void queueDrawEntityBodyOutlines() {
	for (size_t i = 0; i < (size_t)numEntitySlots; i++) {
		if (
			!entitySlots[i].isOccupied ||
			!(entitySlots[i].flags & ENTITY_HAS_BODY)
		) { continue; }
		
		if (entityBodies[i].kind == BODY_SPHERE) {
			queueDrawDebugSphere(entityTransforms[i].pos, entityBodies[i].radius, entityTransforms[i].rot, glm::vec3(0.0f, 1.0f, 0.0f), false, 0.0f);
		}
		else if (entityBodies[i].kind == BODY_AABB) {
			queueDrawDebugCube(entityTransforms[i].pos, entityBodies[i].extents * 2.0f, glm::identity<glm::mat3>(), glm::vec3(0.0f, 1.0f, 0.0f), false, 0.0f);
		}
		else if (entityBodies[i].kind == BODY_OBB) {
			queueDrawDebugCube(entityTransforms[i].pos, entityBodies[i].extents * 2.0f, entityTransforms[i].rot, glm::vec3(0.0f, 1.0f, 0.0f), false, 0.0f);
		}
	}
}

struct EntityMesh {
	Mesh3d const *mesh;
	glm::vec3 offset;
	glm::vec3 scale;
	Tex const *tex;
	glm::vec3 colour;
};

static inline EntityMesh
	entityMeshes[CAP_ENTITY_SLOTS];

static void queueDrawEntityMeshes() {
	for (size_t i = 0; i < (size_t)numEntitySlots; i++) {
		if (
			!entitySlots[i].isOccupied ||
			!(entitySlots[i].flags & ENTITY_HAS_MESH)
		) { continue; }
		
		queueDrawMesh3d(
			entityMeshes[i].mesh,
			glm::translate(entityTransforms[i].pos + entityMeshes[i].offset) *
			glm::mat4(entityTransforms[i].rot) *
			glm::scale(entityMeshes[i].scale),
			entityMeshes[i].tex,
			entityMeshes[i].colour
		);
	}
}

static EntityId entityCreate(
	glm::vec3 pos,
	glm::mat3 rot
) {
	uint8_t slot;
	if (numFreeEntitySlots > 0) {
		slot = freeEntitySlots[--numFreeEntitySlots];
	}
	else {
		assert(numEntitySlots < CAP_ENTITY_SLOTS);
		slot = numEntitySlots++;
	}
	
	entitySlots[slot].isOccupied = true;
	entitySlots[slot].flags = 0;
	
	entityTransforms[slot] = EntityTransform{
		.pos = pos,
		.rot = rot
	};
	
	entityCallbacks[slot] = EntityCallbacks{};
	
	return EntityId{
		.slot = (uint8_t)slot,
		.gen = entitySlots[slot].gen
	};
}

static void entityDestroy(EntityId entity) {
	entitySlots[entity.slot].isOccupied = false;
	entitySlots[entity.slot].gen++;
	
	freeEntitySlots[numFreeEntitySlots++] = entity.slot;
}

static bool entityExists(EntityId entity) {
	return
		entitySlots[entity.slot].isOccupied &&
		entitySlots[entity.slot].gen == entity.gen;
}

static void entityAddCallbacks(EntityId entity, EntityCallbacks const &callbacks) {
	entityCallbacks[entity.slot] = callbacks;
}

static void entityAddSphereBody(
	EntityId entity,
	float radius,
	float density,
	float sFriction, float dFriction,
	float restitution
) {
	entitySlots[entity.slot].flags |= ENTITY_HAS_BODY;
	entityBodies[entity.slot] = bodyCreateSphere(
		entityTransforms[entity.slot].rot,
		radius,
		density,
		sFriction, dFriction,
		restitution
	);
}

static void entityAddAabbBody(
	EntityId entity,
	glm::vec3 size,
	float density,
	float sFriction, float dFriction,
	float restitution
) {
	entitySlots[entity.slot].flags |= ENTITY_HAS_BODY;
	entityBodies[entity.slot] = bodyCreateAabb(
		size,
		density,
		sFriction, dFriction,
		restitution
	);
}

static void entityAddObbBody(
	EntityId entity,
	glm::vec3 size,
	float density,
	float sFriction, float dFriction,
	float restitution
) {
	entitySlots[entity.slot].flags |= ENTITY_HAS_BODY;
	entityBodies[entity.slot] = bodyCreateObb(
		entityTransforms[entity.slot].rot,
		size,
		density,
		sFriction, dFriction,
		restitution
	);
}

static void entityAddStaticSphereBody(
	EntityId entity,
	float radius,
	float sFriction, float dFriction,
	float restitution
) {
	entitySlots[entity.slot].flags |= ENTITY_HAS_BODY;
	entityBodies[entity.slot] = bodyCreateStaticSphere(
		entityTransforms[entity.slot].rot,
		radius,
		sFriction, dFriction,
		restitution
	);
}

static void entityAddStaticAabbBody(
	EntityId entity,
	glm::vec3 size,
	float sFriction, float dFriction,
	float restitution
) {
	entitySlots[entity.slot].flags |= ENTITY_HAS_BODY;
	entityBodies[entity.slot] = bodyCreateStaticAabb(
		size,
		sFriction, dFriction,
		restitution
	);
}

static void entityAddStaticObbBody(
	EntityId entity,
	glm::vec3 size,
	float sFriction, float dFriction,
	float restitution
) {
	entitySlots[entity.slot].flags |= ENTITY_HAS_BODY;
	entityBodies[entity.slot] = bodyCreateStaticObb(
		size,
		sFriction, dFriction,
		restitution
	);
}

static void entityAddMesh(
	EntityId entity,
	Mesh3d const *mesh,
	glm::vec3 offset,
	glm::vec3 scale,
	Tex const *tex,
	glm::vec3 colour
) {
	entitySlots[entity.slot].flags |= ENTITY_HAS_MESH;
	entityMeshes[entity.slot] = EntityMesh{
		.mesh = mesh,
		.offset = offset,
		.scale = scale,
		.tex = tex,
		.colour = colour
	};
}
