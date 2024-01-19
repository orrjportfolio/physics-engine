#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "gfx3d.hpp"

struct Overlap {
	bool exists;
	glm::vec3 norm;
	float depth;
};

static Overlap sphereSphereOverlap(
	glm::vec3 aPos, float aRadius,
	glm::vec3 bPos, float bRadius
) {
	glm::vec3 diff = bPos - aPos;
	float dist = glm::length(diff);
	float sumRadii = aRadius + bRadius;
	
	if (dist < sumRadii) {
		return Overlap{
			.exists = true,
			.norm = -glm::normalize(diff),
			.depth = sumRadii - dist
		};
	}
	
	return Overlap{.exists = false};
}

static Overlap sphereAabbOverlap(
	glm::vec3 spherePos, float sphereRadius,
	glm::vec3 aabbMinPos, glm::vec3 aabbMaxPos
) {
	glm::vec3 p = glm::clamp(spherePos, aabbMinPos, aabbMaxPos);
	glm::vec3 diff = spherePos - p;
	float dist = glm::length(diff);
	
	if (dist < sphereRadius) {
		return Overlap{
			.exists = true,
			.norm = glm::normalize(diff),
			.depth =  sphereRadius - dist
		};
	}
	
	return Overlap{.exists = false};
}

static Overlap sphereObbOverlap(
	glm::vec3 spherePos, float sphereRadius,
	glm::vec3 obbPos, glm::vec3 obbMinPos, glm::vec3 obbMaxPos, glm::mat3 obbRot, glm::mat3 obbRotInv
) {
	glm::mat4 m = glm::translate(obbPos) * glm::mat4(obbRotInv) * glm::translate(-obbPos);
	glm::mat4 mInv = glm::translate(obbPos) * glm::mat4(obbRot) * glm::translate(-obbPos);
	
	glm::vec3 mSpherePos = m * glm::vec4(spherePos, 1.0f);
	glm::vec3 mP = glm::clamp(mSpherePos, obbMinPos, obbMaxPos);
	
	glm::vec3 p = mInv * glm::vec4(mP, 1.0f);
	glm::vec3 diff = spherePos - p;
	float dist = glm::length(diff);
	
	if (dist < sphereRadius) {
		return Overlap{
			.exists = true,
			.norm = glm::normalize(diff),
			.depth =  sphereRadius - dist
		};
	}
	
	return Overlap{.exists = false};
}

static Overlap planePlaneOverlap(
	glm::vec3 const *aVerts,
	glm::vec3 const *bVerts
) {
	return Overlap{.exists = false};
}

static Overlap planeObbOverlap(
	glm::vec3 const *planeVerts,
	glm::vec3 const *obbVerts
) {
	return Overlap{.exists = false};
}

static Overlap aabbAabbOverlap(
	glm::vec3 aPos, glm::vec3 aSize,
	glm::vec3 bPos, glm::vec3 bSize
) {
	return Overlap{.exists = false};
}

static Overlap obbObbOverlap(
	glm::vec3 aPos, glm::vec3 aSize, glm::mat3 aRot,
	glm::vec3 bPos, glm::vec3 bSize, glm::mat3 bRot
) {
	return Overlap{.exists = false};
}
