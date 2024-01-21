#pragma once

#include <assert.h>
#include <stdint.h>

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

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

struct EntityTransform {
	glm::vec3 pos;
	glm::vec3 scale;
	glm::mat3 rot;
};

static inline EntityTransform
	entityTransforms[CAP_ENTITY_SLOTS];

struct EntityMesh {
	Mesh3d const *mesh;
	Tex const *tex;
	glm::vec3 colour;
	glm::vec3 offset;
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
			entityMeshes[i].tex,
			entityMeshes[i].colour,
			entityTransforms[i].pos + entityMeshes[i].offset,
			entityTransforms[i].scale,
			entityTransforms[i].rot
		);
	}
}

struct EntityId {
	uint8_t slot, gen;
};

static EntityId entityCreate(
	glm::vec3 pos,
	glm::vec3 scale,
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
		.scale = scale,
		.rot = rot
	};
	
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

static void entityAddMesh(
	EntityId entity,
	Mesh3d const *mesh,
	Tex const *tex,
	glm::vec3 colour,
	glm::vec3 offset
) {
	entitySlots[entity.slot].flags |= ENTITY_HAS_MESH;
	entityMeshes[entity.slot] = EntityMesh{
		.mesh = mesh,
		.tex = tex,
		.colour = colour,
		.offset = offset
	};
}
