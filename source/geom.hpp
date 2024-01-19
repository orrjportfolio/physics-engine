#pragma once

#include <algorithm>
#include <cstddef>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

static void aabbVerts(glm::vec3 minPos, glm::vec3 maxPos, glm::vec3 *oVerts) {
	oVerts[0] = minPos;
	oVerts[1] = glm::vec3(maxPos.x, minPos.y, minPos.z);
	oVerts[2] = glm::vec3(maxPos.x, maxPos.y, minPos.z);
	oVerts[3] = glm::vec3(minPos.x, maxPos.y, minPos.z);
	oVerts[4] = glm::vec3(minPos.x, minPos.y, maxPos.z);
	oVerts[5] = glm::vec3(maxPos.x, minPos.y, maxPos.z);
	oVerts[6] = glm::vec3(maxPos.x, maxPos.y, maxPos.z);
	oVerts[7] = glm::vec3(minPos.x, maxPos.y, maxPos.z);
}

static void obbVerts(glm::vec3 pos, glm::vec3 size, glm::mat3 rot, glm::vec3 *oVerts) {
	glm::vec3 halfSize = size / 2.0f;
	
	oVerts[0] = pos + (rot * glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z));
	oVerts[1] = pos + (rot * glm::vec3(halfSize.x, -halfSize.y, -halfSize.z));
	oVerts[2] = pos + (rot * glm::vec3(halfSize.x, halfSize.y, -halfSize.z));
	oVerts[3] = pos + (rot * glm::vec3(-halfSize.x, halfSize.y, -halfSize.z));
	oVerts[4] = pos + (rot * glm::vec3(-halfSize.x, -halfSize.y, halfSize.z));
	oVerts[5] = pos + (rot * glm::vec3(halfSize.x, -halfSize.y, halfSize.z));
	oVerts[6] = pos + (rot * glm::vec3(halfSize.x, halfSize.y, halfSize.z));
	oVerts[7] = pos + (rot * glm::vec3(-halfSize.x, halfSize.y, halfSize.z));
}

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

static Overlap spherePlaneOverlap(
	glm::vec3 spherePos, float sphereRadius,
	glm::vec3 planeNorm, float planeDist
) {
	float dist = glm::dot(spherePos, planeNorm) - planeDist;
	
	if (glm::abs(dist) < sphereRadius) {
		return Overlap{
			.exists = true,
			.norm = (dist < 0)? -planeNorm : planeNorm,
			.depth = sphereRadius - glm::abs(dist)
		};
	}
	
	return Overlap{.exists = false};
}

static Overlap aabbAabbOverlap(
	glm::vec3 aMinPos, glm::vec3 aMaxPos,
	glm::vec3 bMinPos, glm::vec3 bMaxPos
) {
	if (
		aMinPos.x < bMaxPos.x && bMinPos.x < aMaxPos.x &&
		aMinPos.y < bMaxPos.y && bMinPos.y < aMaxPos.y &&
		aMinPos.z < bMaxPos.z && bMinPos.z < aMaxPos.z
	) {
		float depthLeft = aMaxPos.x - bMinPos.x;
		float depthRight = bMaxPos.x - aMinPos.x;
		float depthTop = aMaxPos.y - bMinPos.y;
		float depthBottom = bMaxPos.y - aMinPos.y;
		float depthFront = aMaxPos.z - bMinPos.z;
		float depthBack = bMaxPos.z - aMinPos.z;
		
		float depth = std::min({
			depthLeft, depthRight,
			depthTop, depthBottom,
			depthFront, depthBack
		});
		
		glm::vec3 norm;
		if (depth == depthLeft) { norm = glm::vec3(-1.0f, 0.0f, 0.0f); }
		else if (depth == depthRight) { norm = glm::vec3(1.0f, 0.0f, 0.0f); }
		else if (depth == depthTop) { norm = glm::vec3(0.0f, -1.0f, 0.0f); }
		else if (depth == depthBottom) { norm = glm::vec3(0.0f, 1.0f, 0.0f); }
		else if (depth == depthFront) { norm = glm::vec3(0.0f, 0.0f, -1.0f); }
		else if (depth == depthBack) { norm = glm::vec3(0.0f, 0.0f, 1.0f); }
		
		return Overlap{
			.exists = true,
			.norm = norm,
			.depth = depth,
		};
	}
	
	return Overlap{.exists = false};
}

static Overlap obbObbOverlap(
	glm::vec3 aPos, glm::vec3 const *aVerts,
	glm::vec3 bPos, glm::vec3 const *bVerts
) {
	glm::vec3 aAxes[] = {
		glm::normalize(aVerts[1] - aVerts[0]),
		glm::normalize(aVerts[3] - aVerts[0]),
		glm::normalize(aVerts[4] - aVerts[0]),
	};
	
	glm::vec3 bAxes[] = {
		glm::normalize(bVerts[1] - bVerts[0]),
		glm::normalize(bVerts[3] - bVerts[0]),
		glm::normalize(bVerts[4] - bVerts[0]),
	};
	
	glm::vec3 axes[] = {
		aAxes[0],
		aAxes[1],
		aAxes[2],
		bAxes[0],
		bAxes[1],
		bAxes[2],
		glm::normalize(glm::cross(aAxes[0], bAxes[0])),
		glm::normalize(glm::cross(aAxes[0], bAxes[1])),
		glm::normalize(glm::cross(aAxes[0], bAxes[2])),
		glm::normalize(glm::cross(aAxes[1], bAxes[0])),
		glm::normalize(glm::cross(aAxes[1], bAxes[1])),
		glm::normalize(glm::cross(aAxes[1], bAxes[2])),
		glm::normalize(glm::cross(aAxes[2], bAxes[0])),
		glm::normalize(glm::cross(aAxes[2], bAxes[1])),
		glm::normalize(glm::cross(aAxes[2], bAxes[2]))
	};
	
	float minDepth = INFINITY;
	glm::vec3 norm;
	
	for (glm::vec3 axis : axes) {
		auto const projSpan = [](glm::vec3 axis, glm::vec3 const *verts, float *oMin, float *oMax) {
			float min = glm::dot(verts[0], axis);
			float max = min;
			
			for (size_t i = 1; i < 8; i++) {
				float proj = glm::dot(verts[i], axis);
				
				if (min > proj) { min = proj; }
				else if (max < proj) { max = proj; }
			}
			
			*oMin = min;
			*oMax = max;
		};
		
		float aMin, aMax;
		projSpan(axis, aVerts, &aMin, &aMax);
		
		float bMin, bMax;
		projSpan(axis, bVerts, &bMin, &bMax);
		
		if (aMax <= bMin || bMax <= aMin) {
			return Overlap{.exists = false};
		}
		
		float depth = glm::min(aMax - bMin, bMax - aMin);
		
		if (minDepth > depth) {
			minDepth = depth;
			norm = axis;
		}
	}
	
	if (glm::dot(norm, bPos - aPos) > 0.0f) {
		norm = -norm;
	}
	
	return Overlap{
		.exists = true,
		.norm = norm,
		.depth = minDepth
	};
}

static Overlap obbPlaneOverlap(
	glm::vec3 const *obbVerts,
	glm::vec3 planeNorm, float planeDist
) {
	float min = glm::dot(obbVerts[0], planeNorm);
	float max = min;
	
	for (size_t i = 1; i < 8; i++) {
		float proj = glm::dot(obbVerts[i], planeNorm);
		
		if (min > proj) { min = proj; }
		if (max < proj) { max = proj; }
	}
	
	if (min < planeDist && max > planeDist) {
		float depth;
		glm::vec3 norm;
		if (planeDist - min < max - planeDist) {
			depth = planeDist - min;
			norm = -planeNorm;
		}
		else {
			depth = max - planeDist;
			norm = planeNorm;
		}
		
		return Overlap{
			.exists = true,
			.norm = planeNorm,
			.depth = depth
		};
	}
	
	return Overlap{.exists = false};
}
