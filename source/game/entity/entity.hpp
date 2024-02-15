#pragma once

#include <cstddef>
#include <cstdint>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "../gfx/gfx.hpp"
#include "../gfx/scene3d.hpp"

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
		};
		uint8_t asInt;
	};
	
	struct Mesh {
		Mesh3d const *mesh;
		Material const *material;
		glm::vec3 offs, scale;
	};
	
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
	
	static inline glm::vec3
		colliderHalfSizes[CAP];
	
	static inline float
		sFricts[CAP],
		dFricts[CAP],
		bouncinesses[CAP];
	
	static inline glm::vec3
		poses[CAP];
	static inline glm::vec3
		vels[CAP];
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
		invLocalInertiaTensors[CAP];
	static inline glm::mat3
		invInertiaTensors[CAP];
	static inline glm::vec3
		torques[CAP];
	
	static inline Mesh
		meshes[CAP];
	
	uint32_t
		idx, gen;
	
	static void simulateAll(float dt);
	
	static void addAllToScene3d();
	
	static Entity create(glm::vec3 pos, glm::mat3 const &rot = glm::identity<glm::mat3>());
	
	static Entity byIdx(uint32_t idx) {
		return Entity{idx, gens[idx]};
	}
	
	void destroy();
	
	bool exists() {
		return
			flags[idx].asInt != 0 &&
			gens[idx] == gen;
	}
	
	glm::vec3 pos() { return poses[idx]; }
	
	glm::vec3 vel() { return vels[idx]; }
	
	glm::mat3 rot() { return rots[idx]; }
	
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
	}
	
	void setRot(glm::mat3 rot) {
		rots[idx] = rot;
	}
	
	void setRotVel(glm::vec3 rotVel) {
		rotVels[idx] = rotVel;
	}
	
	void makeTrigger(ColliderShape shape);
	
	void makeKinematic(ColliderShape shape, PhysicsMaterial material);
	
	void makeDynamic(ColliderShape shape, PhysicsMaterial material, float density);
	
	void addMesh(
		Mesh3d const &mesh,
		Material const &material,
		glm::vec3 offs = glm::vec3(0.0f),
		glm::vec3 scale = glm::vec3(1.0f)
	) {
		flags[idx].hasMesh = true;
		
		meshes[idx] = Mesh{
			.mesh = &mesh,
			.material = &material,
			.offs = offs, .scale = scale
		};
	}
};
