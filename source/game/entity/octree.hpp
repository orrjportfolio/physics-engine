#pragma once

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

struct Octree {
	struct Entry {
		uint32_t id;
		glm::vec3
			minPos, maxPos;
	};
	
	static constexpr size_t
		SUBDIVIDE_THRESHOLD = 8;
	static constexpr size_t
		MAX_DEPTH = 32;
	
	static Octree root;
	
	Octree *parent;
	size_t depth;
	glm::vec3
		minPos, maxPos;
	Octree *children;
	std::vector<Entry> entries;
	
	static Octree create(Octree *parent, size_t depth, glm::vec3 minPos, glm::vec3 maxPos) {
		return Octree{
			.parent = parent,
			.depth = depth,
			.minPos = minPos,
			.maxPos = maxPos,
			.children = nullptr,
			.entries = std::vector<Entry>()
		};
	}
	
	void destroy() {
		delete[] children;
	}
	
	bool canContain(glm::vec3 entryMinPos, glm::vec3 entryMaxPos) {
		return
			entryMinPos.x < maxPos.x && minPos.x < entryMaxPos.x &&
			entryMinPos.y < maxPos.y && minPos.y < entryMaxPos.y &&
			entryMinPos.z < maxPos.z && minPos.z < entryMaxPos.z;
	}
	
	void overlaps(glm::vec3 testMinPos, glm::vec3 testMaxPos, std::vector<uint32_t> &oOverlaps);
	
	void subdivide();
	
	void unsubdivide();
	
	bool addEntry(
		uint32_t entryId,
		glm::vec3 entryMinPos, glm::vec3 entryMaxPos
	);
	
	bool moveEntry(
		uint32_t entryId,
		glm::vec3 entryPrevMinPos, glm::vec3 entryPrevMaxPos,
		glm::vec3 entryMinPos, glm::vec3 entryMaxPos
	);
	
	bool removeEntry(
		uint32_t entryId,
		glm::vec3 entryMinPos, glm::vec3 entryMaxPos
	);
};
