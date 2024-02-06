#pragma once

#include <algorithm>

#include <glm/glm.hpp>

struct Overlap {
	bool exists;
	glm::vec3 norm;
	float depth;
};

namespace Sphere {
	static Overlap sphereOverlap(
		glm::vec3 aPos, float aRadius,
		glm::vec3 bPos, float bRadius
	) {
		auto diff = bPos - aPos;
		auto dist = glm::length(diff);
		auto sumRadii = aRadius + bRadius;
		
		if (dist < sumRadii) {
			return Overlap{
				.exists = true,
				.norm = -glm::normalize(diff),
				.depth = sumRadii - dist
			};
		}
		
		return Overlap{.exists = false};
	}
	
	static Overlap aabbOverlap(
		glm::vec3 aPos, float aRadius,
		glm::vec3 bMinPos, glm::vec3 bMaxPos
	) {
		auto p = glm::clamp(aPos, bMinPos, bMaxPos);
		auto diff = aPos - p;
		auto dist = glm::length(diff);
		
		if (dist < aRadius) {
			return Overlap{
				.exists = true,
				.norm = glm::normalize(diff),
				.depth = aRadius - dist
			};
		}
		
		return Overlap{.exists = false};
	}
	
	static Overlap obbOverlap(
		glm::vec3 aPos, float aRadius,
		glm::vec3 bMinPosM, glm::vec3 bMaxPosM, glm::mat4 const &bBodyMat, glm::mat4 const &bInvBodyMat
	) {
		auto aPosM = glm::vec3(bBodyMat * glm::vec4(aPos, 1));
		auto pM = glm::clamp(aPosM, bMinPosM, bMaxPosM);
		auto p = glm::vec3(bInvBodyMat * glm::vec4(pM, 1));
		auto diff = aPos - p;
		auto dist = glm::length(diff);
		
		if (dist < aRadius) {
			return Overlap{
				.exists = true,
				.norm = glm::normalize(diff),
				.depth = aRadius - dist
			};
		}
		
		return Overlap{.exists = false};
	}
	
	static Overlap planeOverlap(
		glm::vec3 aPos, float aRadius,
		glm::vec3 bNorm, float bDist
	) {
		auto dist = glm::dot(aPos, bNorm) - bDist;
		auto distAbs = glm::abs(dist);
		
		if (distAbs < aRadius) {
			return Overlap{
				.exists = true,
				.norm = (dist < 0)? -bNorm : bNorm,
				.depth = aRadius - distAbs
			};
		}
		
		return Overlap{.exists = false};
	}
	
	static glm::vec3 sphereContact(
		glm::vec3 aPos, float aRadius,
		glm::vec3 bPos, float bRadius
	) {
		return aPos + (glm::normalize(bPos - aPos) * aRadius);
	}
	
	static glm::vec3 aabbContact(
		glm::vec3 aPos, float aRadius,
		glm::vec3 bMinPos, glm::vec3 bMaxPos
	) {
		return glm::clamp(aPos, bMinPos, bMaxPos);
	}
	
	static glm::vec3 obbContact(
		glm::vec3 aPos, float aRadius,
		glm::vec3 bMinPosM, glm::vec3 bMaxPosM, glm::mat4 const &bBodyMat, glm::mat4 const &bInvBodyMat
	) {
		auto aPosM = glm::vec3(bBodyMat * glm::vec4(aPos, 1));
		auto pM = glm::clamp(aPosM, bMinPosM, bMaxPosM);
		return bInvBodyMat * glm::vec4(pM, 1);
	}
	
	static glm::vec3 planeContact(
		glm::vec3 aPos, float aRadius,
		glm::vec3 bNorm, float bDist
	) {
		auto proj = glm::dot(aPos, bNorm) - bDist;
		
		return aPos + (((proj < 0)? bDist : -bDist) * aRadius);
	}
}

namespace Aabb {
	static void verts(
		glm::vec3 minPos, glm::vec3 maxPos,
		glm::vec3 *oVerts
	) {
		oVerts[0] = minPos;
		oVerts[1] = glm::vec3(maxPos.x, minPos.y, minPos.z);
		oVerts[2] = glm::vec3(maxPos.x, maxPos.y, minPos.z);
		oVerts[3] = glm::vec3(minPos.x, maxPos.y, minPos.z);
		oVerts[4] = glm::vec3(minPos.x, minPos.y, maxPos.z);
		oVerts[5] = glm::vec3(maxPos.x, minPos.y, maxPos.z);
		oVerts[6] = glm::vec3(maxPos.x, maxPos.y, maxPos.z);
		oVerts[7] = glm::vec3(minPos.x, maxPos.y, maxPos.z);
	}
	
	static Overlap sphereOverlap(
		glm::vec3 aMinPos, glm::vec3 aMaxPos,
		glm::vec3 bPos, float bRadius
	) {
		auto o = Sphere::aabbOverlap(bPos, bRadius, aMinPos, aMaxPos);
		if (o.exists) { o.norm *= -1; }
		return o;
	}
	
	static Overlap aabbOverlap(
		glm::vec3 aMinPos, glm::vec3 aMaxPos,
		glm::vec3 bMinPos, glm::vec3 bMaxPos
	) {
		if (
			aMinPos.x >= bMaxPos.x || bMinPos.x >= aMaxPos.x ||
			aMinPos.y >= bMaxPos.y || bMinPos.y >= aMaxPos.y ||
			aMinPos.z >= bMaxPos.z || bMinPos.z >= aMaxPos.z
		) {
			return Overlap{.exists = false};
		}
		
		auto depthLeft = aMaxPos.x - bMinPos.x;
		auto depthRight = bMaxPos.x - aMinPos.x;
		auto depthTop = aMaxPos.y - bMinPos.y;
		auto depthBottom = bMaxPos.y - aMinPos.y;
		auto depthFront = aMaxPos.z - bMinPos.z;
		auto depthBack = bMaxPos.z - aMinPos.z;
		
		auto depth = std::min({
			depthLeft, depthRight,
			depthTop, depthBottom,
			depthFront, depthBack,
		});
		auto norm =
			(depth == depthLeft)? glm::vec3(-1, 0, 0) :
			(depth == depthRight)? glm::vec3(1, 0, 0) :
			(depth == depthTop)? glm::vec3(0, -1, 0) :
			(depth == depthBottom)? glm::vec3(0, 1, 0) :
			(depth == depthFront)? glm::vec3(0, 0, -1) :
			glm::vec3(0, 0, 1);
		
		return Overlap{
			.exists = true,
			.norm = norm,
			.depth = depth
		};
	}
	
	static Overlap planeOverlap(
		glm::vec3 aPos, glm::vec3 aHalfExtents,
		glm::vec3 bNorm, float bDist
	) {
		auto r = glm::dot(aHalfExtents, bNorm);
		
		auto dist = glm::dot(aPos, bNorm) - bDist;
		auto distAbs = glm::abs(dist);
		
		if (distAbs < r) {
			return Overlap{
				.exists = true,
				.norm = (dist < 0)? -bNorm : bNorm,
				.depth = r - distAbs
			};
		}
		
		return Overlap{.exists = false};
	}
	
	static glm::vec3 sphereContact(
		glm::vec3 aMinPos, glm::vec3 aMaxPos,
		glm::vec3 bPos, float bRadius
	) {
		return Sphere::aabbContact(bPos, bRadius, aMinPos, aMaxPos);
	}
	
	static void aabbContacts(
		glm::vec3 aMinPos, glm::vec3 aMaxPos,
		glm::vec3 bMinPos, glm::vec3 bMaxPos,
		glm::vec3 *oContacts
	) {
		auto epsilon = 1 / 128.0f;
		
		if (glm::abs(aMinPos.x - bMaxPos.x) < epsilon) {
			oContacts[0] = glm::vec3(aMinPos.x, glm::max(aMinPos.y, bMinPos.y), glm::max(aMinPos.z, bMinPos.z));
			oContacts[1] = glm::vec3(aMinPos.x, glm::min(aMaxPos.y, bMaxPos.y), glm::max(aMinPos.z, bMinPos.z));
			oContacts[2] = glm::vec3(aMinPos.x, glm::min(aMaxPos.y, bMaxPos.y), glm::min(aMaxPos.z, bMaxPos.z));
			oContacts[3] = glm::vec3(aMinPos.x, glm::max(aMinPos.y, bMinPos.y), glm::min(aMaxPos.z, bMaxPos.z));
		}
		else if (glm::abs(bMinPos.x - aMaxPos.x) < epsilon) {
			oContacts[0] = glm::vec3(aMaxPos.x, glm::max(aMinPos.y, bMinPos.y), glm::max(aMinPos.z, bMinPos.z));
			oContacts[1] = glm::vec3(aMaxPos.x, glm::min(aMaxPos.y, bMaxPos.y), glm::max(aMinPos.z, bMinPos.z));
			oContacts[2] = glm::vec3(aMaxPos.x, glm::min(aMaxPos.y, bMaxPos.y), glm::min(aMaxPos.z, bMaxPos.z));
			oContacts[3] = glm::vec3(aMaxPos.x, glm::max(aMinPos.y, bMinPos.y), glm::min(aMaxPos.z, bMaxPos.z));
		}
		else if (glm::abs(aMinPos.y - bMaxPos.y) < epsilon) {
			oContacts[0] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), aMinPos.y, glm::max(aMinPos.z, bMinPos.z));
			oContacts[1] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), aMinPos.y, glm::max(aMinPos.z, bMinPos.z));
			oContacts[2] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), aMinPos.y, glm::min(aMaxPos.z, bMaxPos.z));
			oContacts[3] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), aMinPos.y, glm::min(aMaxPos.z, bMaxPos.z));
		}
		else if (glm::abs(bMinPos.y - aMaxPos.y) < epsilon) {
			oContacts[0] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), aMaxPos.y, glm::max(aMinPos.z, bMinPos.z));
			oContacts[1] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), aMaxPos.y, glm::max(aMinPos.z, bMinPos.z));
			oContacts[2] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), aMaxPos.y, glm::min(aMaxPos.z, bMaxPos.z));
			oContacts[3] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), aMaxPos.y, glm::min(aMaxPos.z, bMaxPos.z));
		}
		else if (glm::abs(aMinPos.z - bMaxPos.z) < epsilon) {
			oContacts[0] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), glm::max(aMinPos.y, bMinPos.y), aMinPos.z);
			oContacts[1] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), glm::max(aMinPos.y, bMinPos.y), aMinPos.z);
			oContacts[2] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), glm::min(aMaxPos.y, bMaxPos.y), aMinPos.z);
			oContacts[3] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), glm::min(aMaxPos.y, bMaxPos.y), aMinPos.z);
		}
		else if (glm::abs(bMinPos.z - aMaxPos.z) < epsilon) {
			oContacts[0] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), glm::max(aMinPos.y, bMinPos.y), aMaxPos.z);
			oContacts[1] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), glm::max(aMinPos.y, bMinPos.y), aMaxPos.z);
			oContacts[2] = glm::vec3(glm::min(aMaxPos.x, bMaxPos.x), glm::min(aMaxPos.y, bMaxPos.y), aMaxPos.z);
			oContacts[3] = glm::vec3(glm::max(aMinPos.x, bMinPos.x), glm::min(aMaxPos.y, bMaxPos.y), aMaxPos.z);
		}
	}
}

namespace Obb {
	static void verts(
		glm::vec3 pos, glm::vec3 halfSize, glm::mat3 const &rot,
		glm::vec3 *oVerts
	) {
		oVerts[0] = pos + (rot * glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z));
		oVerts[1] = pos + (rot * glm::vec3(halfSize.x, -halfSize.y, -halfSize.z));
		oVerts[2] = pos + (rot * glm::vec3(halfSize.x, halfSize.y, -halfSize.z));
		oVerts[3] = pos + (rot * glm::vec3(-halfSize.x, halfSize.y, -halfSize.z));
		oVerts[4] = pos + (rot * glm::vec3(-halfSize.x, -halfSize.y, halfSize.z));
		oVerts[5] = pos + (rot * glm::vec3(halfSize.x, -halfSize.y, halfSize.z));
		oVerts[6] = pos + (rot * glm::vec3(halfSize.x, halfSize.y, halfSize.z));
		oVerts[7] = pos + (rot * glm::vec3(-halfSize.x, halfSize.y, halfSize.z));
	}
	
	static Overlap sphereOverlap(
		glm::vec3 aMinPosM, glm::vec3 aMaxPosM, glm::mat4 const &aModelMat, glm::mat4 const &aInvModelMat,
		glm::vec3 bPos, float bRadius
	) {
		auto o = Sphere::obbOverlap(bPos, bRadius, aMinPosM, aMaxPosM, aModelMat, aInvModelMat);
		if (o.exists) { o.norm *= -1; }
		return o;
	}
	
	static Overlap obbOverlap(
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
		
		auto minDepth = (float)INFINITY;
		glm::vec3 norm;
		
		for (auto axis : axes) {
			auto projSpan = [](glm::vec3 axis, glm::vec3 const *verts, float *oMin, float *oMax) {
				auto min = glm::dot(verts[0], axis);
				auto max = min;
				
				for (int i = 0; i < 8; i++) {
					auto proj = glm::dot(verts[i], axis);
					
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
			
			auto depth = glm::min(aMax - bMin, bMax - aMin);
			
			if (minDepth > depth) {
				minDepth = depth;
				norm = axis;
			}
		}
		
		if (glm::dot(norm, bPos - aPos) > 0) {
			norm = -norm;
		}
		
		return Overlap{
			.exists = true,
			.norm = norm,
			.depth = minDepth
		};
	}
	
	static Overlap planeOverlap(
		glm::vec3 const *aVerts,
		glm::vec3 bNorm, float bDist
	) {
		auto min = glm::dot(aVerts[0], bNorm);
		auto max = min;
		
		for (int i = 0; i < 8; i++) {
			auto proj = dot(aVerts[i], bNorm);
			
			if (min > proj) { min = proj; }
			if (max < proj) { max = proj; }
		}
		
		if (min < bDist && max > bDist) {
			float depth;
			glm::vec3 norm;
			if (bDist - min < max - bDist) {
				depth = bDist - min;
				norm = bNorm;
			}
			else {
				depth = max - bDist;
				norm = -bNorm;
			}
			
			return Overlap{
				.exists = true,
				.norm = norm,
				.depth = depth
			};
		}
		
		return Overlap{.exists = false};
	}
	
	static glm::vec3 sphereContact(
		glm::vec3 aMinPosM, glm::vec3 aMaxPosM, glm::mat4 const &aModelMat, glm::mat4 const &aInvModelMat,
		glm::vec3 bPos, float bRadius
	) {
		return Sphere::obbContact(bPos, bRadius, aMinPosM, aMaxPosM, aModelMat, aInvModelMat);
	}
	
	static size_t obbContacts(
		glm::vec3 aMinPosM, glm::vec3 aMaxPosM, glm::mat4 const &aModelMat, glm::vec3 const *aVerts,
		glm::vec3 bMinPosM, glm::vec3 bMaxPosM, glm::mat4 const &bBodyMat, glm::vec3 const *bVerts,
		glm::vec3 *oContacts
	) {
		auto epsilon = 1 / 128.0f;
		
		size_t numContacts = 0;
		glm::vec3 contacts[16];
		
		auto getFaceContacts = [&](
			glm::vec3 const *aVerts,
			glm::vec3 bMinPos, glm::vec3 bMaxPos, glm::mat3 const &bBodyMat
		) {
			for (int i = 0; i < 8 && numContacts < 16; i++) {
				auto vM = bBodyMat * aVerts[i];
				auto pM = glm::clamp(vM, bMinPos, bMaxPos);
				auto diffM = glm::abs(vM - pM);
				
				if (diffM.x < epsilon && diffM.y < epsilon && diffM.z < epsilon) {
					contacts[numContacts++] = aVerts[i];
				}
			}
		};
		
		getFaceContacts(aVerts, bMinPosM, bMaxPosM, bBodyMat);
		getFaceContacts(bVerts, aMinPosM, aMaxPosM, aModelMat);
		
		int edges[][2] = {
			{0, 1}, {1, 2}, {2, 3}, {3, 0},
			{4, 5}, {5, 6}, {6, 7}, {7, 4},
			{0, 4}, {1, 5}, {2, 6}, {3, 7}
		};
		
		for (int i = 0; i < 12 && numContacts < 16; i++) {
			for (int j = 0; j < 12 && numContacts < 16; j++) {
				auto aEdge = edges[i];
				auto bEdge = edges[j];
				auto a1 = aVerts[aEdge[0]], a2 = aVerts[aEdge[1]];
				auto b1 = bVerts[bEdge[0]], b2 = bVerts[bEdge[1]];
				
				auto u = a2 - a1;
				auto v = b2 - b1;
				auto w = a1 - b1;
				
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
				if (glm::length(vec) < epsilon) {
					contacts[numContacts++] = a1 + (sc * (a2 - a1));
				}
			}
		}
		
		size_t i = 0;
		for (; i < ((numContacts < 8)? numContacts : 8); i++) {
			for (size_t j = i + 1; j < numContacts; j++) {
				auto diff = glm::abs(contacts[j] - contacts[i]);
				
				if (diff.x < epsilon && diff.y < epsilon && diff.z < epsilon) {
					contacts[j--] = contacts[--numContacts];
				}
			}
			
			oContacts[i] = contacts[i];
		}
		
		return i;
	}
	
	static size_t planeContacts(
		glm::vec3 const *aVerts,
		glm::vec3 bNorm, float bDist,
		glm::vec3 *oContacts
	) {
		float epsilon = 1 / 128.0f;
		
		size_t numContacts = 0;
		
		for (int i = 0; i < 8; i++) {
			float dist = glm::abs(glm::dot(aVerts[i], bNorm) - bDist);
			if (dist < epsilon) {
				oContacts[numContacts++] = aVerts[i];
			}
		}
		
		return numContacts;
	}
}
