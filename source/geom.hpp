#pragma once

#include <algorithm>
#include <cstddef>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

constexpr float EPSILON = 1.0f / 1024.0f;

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
	glm::vec3 obbPos, glm::vec3 obbMinPos, glm::vec3 obbMaxPos, glm::mat3 const *obbRot, glm::mat3 const *obbRotInv
) {
	glm::mat4 m = glm::translate(obbPos) * glm::mat4(*obbRotInv) * glm::translate(-obbPos);
	glm::mat4 mInv = glm::translate(obbPos) * glm::mat4(*obbRot) * glm::translate(-obbPos);
	
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

static Overlap aabbPlaneOverlap(
	glm::vec3 aabbPos, glm::vec3 aabbSize,
	glm::vec3 planeNorm, float planeDist
) {
	float r = glm::dot(aabbSize / 2.0f, planeNorm);
	
	float dist = glm::dot(aabbPos, planeNorm) - planeDist;
	
	if (glm::abs(dist) < r) {
		return Overlap{
			.exists = true,
			.norm = (dist < 0.0f)? -planeNorm : planeNorm,
			.depth = r - glm::abs(dist)
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
			norm = planeNorm;
		}
		else {
			depth = max - planeDist;
			norm = -planeNorm;
		}
		
		return Overlap{
			.exists = true,
			.norm = norm,
			.depth = depth
		};
	}
	
	return Overlap{.exists = false};
}

static glm::vec3 sphereSphereContact(
	glm::vec3 aPos, float aRadius,
	glm::vec3 bPos, float bRadius
) {
	return aPos + (glm::normalize(bPos - aPos) * aRadius);
}

static glm::vec3 sphereAabbContact(
	glm::vec3 spherePos, float sphereRadius,
	glm::vec3 aabbMinPos, glm::vec3 aabbMaxPos
) {
	return glm::clamp(spherePos, aabbMinPos, aabbMaxPos);
}

static glm::vec3 sphereObbContact(
	glm::vec3 spherePos, float sphereRadius,
	glm::vec3 obbPos, glm::vec3 obbMinPos, glm::vec3 obbMaxPos, glm::mat3 const *obbRot, glm::mat3 const *obbRotInv
) {
	glm::mat4 m = glm::translate(obbPos) * glm::mat4(*obbRotInv) * glm::translate(-obbPos);
	glm::mat4 mInv = glm::translate(obbPos) * glm::mat4(*obbRot) * glm::translate(-obbPos);
	
	glm::vec3 mSpherePos = m * glm::vec4(spherePos, 1.0f);
	glm::vec3 mP = glm::clamp(mSpherePos, obbMinPos, obbMaxPos);
	
	return mInv * glm::vec4(mP, 1.0f);
}

static glm::vec3 spherePlaneContact(
	glm::vec3 spherePos, float sphereRadius,
	glm::vec3 planeNorm, float planeDist
) {
	float proj = glm::dot(spherePos, planeNorm) - planeDist;
	
	return spherePos + (((proj < 0.0f)? planeNorm : -planeNorm) * sphereRadius);
}

static void aabbAabbContacts(
	glm::vec3 aMinPos, glm::vec3 aMaxPos,
	glm::vec3 bMinPos, glm::vec3 bMaxPos,
	glm::vec3 *oContacts
) {
	if (glm::abs(aMinPos.x - bMaxPos.x) < EPSILON) {
		oContacts[0] = glm::vec3(aMinPos.x, glm::max(aMinPos.y, bMinPos.y), glm::max(aMinPos.z, bMinPos.z));
		oContacts[1] = glm::vec3(aMinPos.x, glm::min(aMaxPos.y, bMaxPos.y), glm::max(aMinPos.z, bMinPos.z));
		oContacts[2] = glm::vec3(aMinPos.x, glm::min(aMaxPos.y, bMaxPos.y), glm::min(aMaxPos.z, bMaxPos.z));
		oContacts[3] = glm::vec3(aMinPos.x, glm::max(aMinPos.y, bMinPos.y), glm::min(aMaxPos.z, bMaxPos.z));
	}
	else if (glm::abs(bMinPos.x - aMaxPos.x) < EPSILON) {
		oContacts[0] = glm::vec3(aMaxPos.x, glm::max(aMinPos.y, bMinPos.y), glm::max(aMinPos.z, bMinPos.z));
		oContacts[1] = glm::vec3(aMaxPos.x, glm::min(aMaxPos.y, bMaxPos.y), glm::max(aMinPos.z, bMinPos.z));
		oContacts[2] = glm::vec3(aMaxPos.x, glm::min(aMaxPos.y, bMaxPos.y), glm::min(aMaxPos.z, bMaxPos.z));
		oContacts[3] = glm::vec3(aMaxPos.x, glm::max(aMinPos.y, bMinPos.y), glm::min(aMaxPos.z, bMaxPos.z));
	}
	else if (glm::abs(aMinPos.y - bMaxPos.y) < EPSILON) {
		oContacts[0] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), aMinPos.y, glm::max(aMinPos.z, bMinPos.z));
		oContacts[1] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), aMinPos.y, glm::max(aMinPos.z, bMinPos.z));
		oContacts[2] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), aMinPos.y, glm::min(aMaxPos.z, bMaxPos.z));
		oContacts[3] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), aMinPos.y, glm::min(aMaxPos.z, bMaxPos.z));
	}
	else if (glm::abs(bMinPos.y - aMaxPos.y) < EPSILON) {
		oContacts[0] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), aMaxPos.y, glm::max(aMinPos.z, bMinPos.z));
		oContacts[1] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), aMaxPos.y, glm::max(aMinPos.z, bMinPos.z));
		oContacts[2] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), aMaxPos.y, glm::min(aMaxPos.z, bMaxPos.z));
		oContacts[3] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), aMaxPos.y, glm::min(aMaxPos.z, bMaxPos.z));
	}
	else if (glm::abs(aMinPos.z - bMaxPos.z) < EPSILON) {
		oContacts[0] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), glm::max(aMinPos.y, bMinPos.y), aMinPos.z);
		oContacts[1] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), glm::max(aMinPos.y, bMinPos.y), aMinPos.z);
		oContacts[2] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), glm::min(aMaxPos.y, bMaxPos.y), aMinPos.z);
		oContacts[3] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), glm::min(aMaxPos.y, bMaxPos.y), aMinPos.z);
	}
	else if (glm::abs(bMinPos.z - aMaxPos.z) < EPSILON) {
		oContacts[0] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), glm::max(aMinPos.y, bMinPos.y), aMaxPos.z);
		oContacts[1] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), glm::max(aMinPos.y, bMinPos.y), aMaxPos.z);
		oContacts[2] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), glm::min(aMaxPos.y, bMaxPos.y), aMaxPos.z);
		oContacts[3] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), glm::min(aMaxPos.y, bMaxPos.y), aMaxPos.z);
	}
}

static size_t obbObbContacts(
	glm::vec3 aPos, glm::vec3 aMinPos, glm::vec3 aMaxPos, glm::mat3 const *aRotInv, glm::vec3 const *aVerts,
	glm::vec3 bPos, glm::vec3 bMinPos, glm::vec3 bMaxPos, glm::mat3 const *bRotInv, glm::vec3 const *bVerts,
	glm::vec3 *oContacts
) {
	size_t numContacts = 0;
	glm::vec3 contacts[16];
	
	auto const f = [&](
		glm::vec3 const *aVerts,
		glm::vec3 bPos, glm::vec3 bMinPos, glm::vec3 bMaxPos, glm::mat3 const *bRotInv
	) {
		glm::mat4 m = glm::translate(bPos) * glm::mat4(*bRotInv) * glm::translate(-bPos);
		
		for (size_t i = 0; i < 8; i++) {
			glm::vec3 mV = m * glm::vec4(aVerts[i], 1.0f);
			glm::vec3 mP = glm::clamp(mV, bMinPos, bMaxPos);
			glm::vec3 mDiff = glm::abs(mP - mV);
			if (mDiff.x < 0.01 && mDiff.y < 0.01 && mDiff.z < 0.01) {
				if (numContacts == 16) { break; }
				
				contacts[numContacts++] = aVerts[i];
			}
		}
	};
	
	f(aVerts, bPos, bMinPos, bMaxPos, bRotInv);
	f(bVerts, aPos, aMinPos, aMaxPos, aRotInv);
	
	std::pair<size_t, size_t> edges[] = {
		std::pair<size_t, size_t>(0, 1),
		std::pair<size_t, size_t>(1, 2),
		std::pair<size_t, size_t>(2, 3),
		std::pair<size_t, size_t>(3, 0),
		
		std::pair<size_t, size_t>(4, 5),
		std::pair<size_t, size_t>(5, 6),
		std::pair<size_t, size_t>(6, 7),
		std::pair<size_t, size_t>(7, 4),
		
		std::pair<size_t, size_t>(0, 4),
		std::pair<size_t, size_t>(1, 5),
		std::pair<size_t, size_t>(2, 6),
		std::pair<size_t, size_t>(3, 7),
	};
	
	for (size_t i = 0; i < 12; i++) {
		for (size_t j = 0; j < 12; j++) {
			std::pair<size_t, size_t> edgeA = edges[i];
			std::pair<size_t, size_t> edgeB = edges[j];
			glm::vec3 a1 = aVerts[edgeA.first], a2 = aVerts[edgeA.second];
			glm::vec3 b1 = bVerts[edgeB.first], b2 = bVerts[edgeB.second];
			
			glm::vec3 u = a2 - a1;
			glm::vec3 v = b2 - b1;
			glm::vec3 w = a1 - b1;
			
			float a = glm::dot(u, u);
			float b = glm::dot(u, v);
			float c = glm::dot(v, v);
			float d = glm::dot(u, w);
			float e = glm::dot(v, w);
			
			float dd = (a * c) - (b * b);
			
			float sc, tc;
			if (dd < 1e-6) {
				sc = 0.0f;
				tc = (b > c)? (d / b) : (e / c);
			}
			else {
				sc = ((b * e) - (c * d)) / dd;
				tc = ((a * e) - (b * d)) / dd;
			}
			
			sc = glm::clamp(sc, 0.0f, 1.0f);
			tc = glm::clamp(tc, 0.0f, 1.0f);
			
			glm::vec3 vec = w + (sc * u) - (tc * v);
			
			if (glm::length(vec) < 0.01) {
				if (numContacts == 16) { break; }
				
				contacts[numContacts++] = a1 + (sc * (a2 - a1));
			}
		}
	}
	
	for (size_t i = 0; i < glm::min(numContacts, (size_t)4); i++) {
		for (size_t j = i + 1; j < numContacts; j++) {
			glm::vec3 diff = glm::abs(contacts[j] - contacts[i]);
			
			if (diff.x < 0.01 && diff.y < 0.01 && diff.z < 0.01) {
				contacts[j--] = contacts[--numContacts];
			}
		}
		
		oContacts[i] = contacts[i];
	}
	
	return glm::min(numContacts, (size_t)4);
}

static size_t obbPlaneContacts(
	glm::vec3 const *obbVerts,
	glm::vec3 planeNorm, float planeDist,
	glm::vec3 *oContacts
) {
	size_t numContacts = 0;
	
	for (size_t i = 0; i < 8; i++) {
		float dist = glm::abs(glm::dot(obbVerts[i], planeNorm) - planeDist);
		if (dist < EPSILON) {
			oContacts[numContacts++] = obbVerts[i];
			
			if (numContacts == 4) { break; }
		}
	}
	
	return numContacts;
}
