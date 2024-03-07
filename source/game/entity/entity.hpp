#pragma once

#include <cstddef>
#include <cstdint>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "../gfx/gfx.hpp"
#include "../gfx/scene3d.hpp"

extern bool useEuler;

enum ColliderKind {
	COLLIDER_KIND_NONE,
	COLLIDER_KIND_TRIGGER,
	COLLIDER_KIND_KINEMATIC,
	COLLIDER_KIND_DYNAMIC
};

struct ColliderShape {
	enum Kind : uint8_t {
		KIND_SPHERE,
		KIND_BOX
	};
	
	Kind kind;
	glm::vec3 halfSize;
	bool isAxisAligned;
	
	static ColliderShape sphere(float radius) {
		return ColliderShape{
			.kind = KIND_SPHERE,
			.halfSize = glm::vec3(radius),
			.isAxisAligned = false
		};
	}
	
	static ColliderShape axisAlignedBox(glm::vec3 halfSize) {
		return ColliderShape{
			.kind = KIND_BOX,
			.halfSize = halfSize,
			.isAxisAligned = true
		};
	}
	
	static ColliderShape box(glm::vec3 halfSize) {
		return ColliderShape{
			.kind = KIND_BOX,
			.halfSize = halfSize,
			.isAxisAligned = false
		};
	}
};

struct PhysicsMaterial {
	float
		sFrict, dFrict,
		bounciness;
};

struct Entity {
	union Flags {
		struct {
			uint8_t hasMesh : 1;
			uint8_t colliderKind : 2;
			uint8_t colliderShapeKind : 1;
			uint8_t colliderIsAxisAligned : 1;
			uint8_t isSleeping : 1;
		};
		uint8_t asInt;
	};
	
	using UpdateFunc = void(*)(Entity e, float dt);
	using CollideFunc = void(*)(Entity e, glm::vec3 eVel, Entity other, glm::vec3 otherVel, glm::vec3 norm);
	using TriggerFunc = void(*)(Entity e, Entity other);
	
	static constexpr uint32_t
		CAP = 1024;
	static inline uint32_t
		num;
		
	static inline Flags
		flags[CAP];
	static inline uint32_t
		gens[CAP];
	
	static inline uint32_t
		numEmpties;
	static inline uint32_t
		empties[CAP];
	
	static inline float
		lifetimes[CAP];
	
	static inline glm::vec3
		colliderHalfSizes[CAP];
	
	static inline float
		sFricts[CAP],
		dFricts[CAP],
		bouncinesses[CAP];
	
	static inline glm::vec3
		poses[CAP];
	static inline glm::vec3
		prevPoses[CAP];
	static inline glm::vec3
		vels[CAP];
	static inline glm::vec3
		accels[CAP];
	static inline float
		invMasses[CAP];
	static inline glm::vec3
		forces[CAP];
	
	static inline glm::mat3
		rots[CAP];
	static inline glm::vec3
		rotMoms[CAP];
	static inline glm::vec3
		rotVels[CAP];
	static inline glm::vec3
		rotAccels[CAP];
	static inline glm::vec3
		invLocalInertiaTensors[CAP];
	static inline glm::mat3
		invInertiaTensors[CAP];
	static inline glm::vec3
		torques[CAP];
	
	static inline float
		restingTimes[CAP];
	
	static inline Mesh3d
		*meshes[CAP];
	static inline Material
		*meshMaterials[CAP];
	static inline glm::mat4
		meshMats[CAP];
	
	static inline UpdateFunc
		updateFuncs[CAP];
	static inline CollideFunc
		collideFuncs[CAP];
	static inline TriggerFunc
		triggerFuncs[CAP];
	
	uint32_t
		idx, gen;
	
	static void updateAll(float dt);
	
	static void simulateAll(float dt);
	
	static void wakeUpAllInRegion(glm::vec3 min, glm::vec3 max);
	
	static void addAllToScene3d();
	
	static Entity create(glm::vec3 pos, glm::mat3 const &rot = glm::identity<glm::mat3>());
	
	static Entity byIdx(uint32_t idx) {
		return Entity{idx, gens[idx]};
	}
	
	static void destroyAll();
	
	void destroy();
	
	bool exists() {
		return
			flags[idx].asInt != 0 &&
			gens[idx] == gen;
	}
	
	glm::vec3 pos() { return poses[idx]; }
	
	glm::vec3 vel() { return vels[idx]; }
	
	glm::mat3 rot() { return rots[idx]; }
	
	glm::vec3 right() { return rots[idx][0]; }
	
	glm::vec3 up() { return rots[idx][1]; }
	
	glm::vec3 forward() { return rots[idx][2]; }
	
	glm::vec3 rotVel() { return rotVels[idx]; }
	
	glm::mat4 bodyMat() {
		return
			glm::translate(poses[idx]) *
			glm::mat4(glm::transpose(rots[idx])) *
			glm::translate(-poses[idx]);
	}
	
	glm::mat4 bodyMatInv() {
		return
			glm::translate(poses[idx]) *
			glm::mat4(rots[idx]) *
			glm::translate(-poses[idx]);
	}
	
	ColliderShape colliderShape() {
		return ColliderShape{
			.kind = (ColliderShape::Kind)flags[idx].colliderKind,
			.halfSize = colliderHalfSizes[idx],
			.isAxisAligned = (bool)flags[idx].colliderIsAxisAligned
		};
	}
	
	void colliderBounds(glm::vec3 *oMin, glm::vec3 *oMax) {
		glm::vec3 r = colliderHalfSizes[idx];
		if (
			flags[idx].colliderShapeKind == ColliderShape::KIND_BOX &&
			!flags[idx].colliderIsAxisAligned
		) {
			r = glm::vec3(std::max({r.x, r.y, r.z}) * 1.732051f);
		}
		
		*oMin = poses[idx] - r;
		*oMax = poses[idx] + r;
	}
	
	void setPos(glm::vec3 pos);
	
	void setVel(glm::vec3 vel) {
		vels[idx] = vel;
		auto k = flags[idx].colliderKind;
		if (k == COLLIDER_KIND_KINEMATIC || k == COLLIDER_KIND_DYNAMIC) {
			flags[idx].isSleeping = false;
		}
	}
	
	void setRot(glm::mat3 rot) {
		rots[idx] = rot;
		auto k = flags[idx].colliderKind;
		if (k == COLLIDER_KIND_KINEMATIC || k == COLLIDER_KIND_DYNAMIC) {
			flags[idx].isSleeping = false;
		}
	}
	
	void setRotVel(glm::vec3 rotVel) {
		rotVels[idx] = rotVel;
		auto k = flags[idx].colliderKind;
		if (k == COLLIDER_KIND_KINEMATIC || k == COLLIDER_KIND_DYNAMIC) {
			flags[idx].isSleeping = false;
		}
	}
	
	void addForce(glm::vec3 force) {
		flags[idx].isSleeping = false;
		forces[idx] += force;
	}
	
	void addForceAt(glm::vec3 force, glm::vec3 pos) {
		flags[idx].isSleeping = false;
		forces[idx] += force;
		torques[idx] += glm::cross(pos - poses[idx], force);
	}
	
	void makeTrigger(ColliderShape shape);
	
	void makeKinematic(ColliderShape shape, PhysicsMaterial material);
	
	void makeDynamic(ColliderShape shape, PhysicsMaterial material, float density);
	
	void addMesh(
		Mesh3d &mesh,
		Material &material,
		glm::mat4 mat = glm::identity<glm::mat4>()
	) {
		flags[idx].hasMesh = true;
		
		meshes[idx] = &mesh;
		meshMaterials[idx] = &material;
		meshMats[idx] = mat;
	}
	
	void setUpdateFunc(UpdateFunc func) {
		updateFuncs[idx] = func;
	}
	
	void setCollideFunc(CollideFunc func) {
		collideFuncs[idx] = func;
	}
	
	void setTriggerFunc(TriggerFunc func) {
		triggerFuncs[idx] = func;
	}
};
