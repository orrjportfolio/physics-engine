#include "octree.hpp"

Octree Octree::root = Octree::create(nullptr, 0, glm::vec3(-200.0f), glm::vec3(500.0f));

void Octree::overlaps(glm::vec3 testMinPos, glm::vec3 testMaxPos, std::vector<uint32_t> &oOverlaps) {
	if (
		minPos.x > testMaxPos.x || testMinPos.x > maxPos.x ||
		minPos.y > testMaxPos.y || testMinPos.y > maxPos.y ||
		minPos.z > testMaxPos.z || testMinPos.z > maxPos.z
	) {
		return;
	}
	
	for (auto e : entries) {
		if (
			e.minPos.x < testMaxPos.x && testMinPos.x < e.maxPos.x ||
			e.minPos.y < testMaxPos.y && testMinPos.y < e.maxPos.y ||
			e.minPos.z < testMaxPos.z && testMinPos.z < e.maxPos.z
		) {
			oOverlaps.push_back(e.id);
		}
	}
	
	if (children != nullptr) {
		for (int i = 0; i < 8; i++) {
			children[i].overlaps(testMinPos, testMaxPos, oOverlaps);
		}
	}
}

void Octree::subdivide() {
	auto
		a = minPos,
		b = (minPos + maxPos) / 2.0f,
		c = maxPos;
	
	auto newDepth = depth + 1;
	
	children = new Octree[] {
		Octree::create(this, newDepth, glm::vec3(a.x, a.y, a.z), glm::vec3(b.x, b.y, b.z)),
		Octree::create(this, newDepth, glm::vec3(a.x, a.y, b.z), glm::vec3(b.x, b.y, c.z)),
		Octree::create(this, newDepth, glm::vec3(a.x, b.y, a.z), glm::vec3(b.x, c.y, b.z)),
		Octree::create(this, newDepth, glm::vec3(a.x, b.y, b.z), glm::vec3(b.x, c.y, c.z)),
		Octree::create(this, newDepth, glm::vec3(b.x, a.y, a.z), glm::vec3(c.x, b.y, b.z)),
		Octree::create(this, newDepth, glm::vec3(b.x, a.y, b.z), glm::vec3(c.x, b.y, c.z)),
		Octree::create(this, newDepth, glm::vec3(b.x, b.y, a.z), glm::vec3(c.x, c.y, b.z)),
		Octree::create(this, newDepth, glm::vec3(b.x, b.y, b.z), glm::vec3(c.x, c.y, c.z))
	};
}

void Octree::unsubdivide() {
	
}

bool Octree::addEntry(
	uint32_t entryId,
	glm::vec3 entryMinPos, glm::vec3 entryMaxPos
) {
	if (!canContain(entryMinPos, entryMaxPos)) {
		return false;
	}
	
	if (children != nullptr) {
		for (int i = 0; i < 8; i++) {
			if (children[i].addEntry(entryId, entryMinPos, entryMaxPos)) {
				return true;
			}
		}
	}
	
	entries.push_back(Entry{
		.id = entryId,
		.minPos = entryMinPos,
		.maxPos = entryMaxPos
	});
	
	if (
		children == nullptr &&
		entries.size() >= SUBDIVIDE_THRESHOLD &&
		depth < MAX_DEPTH
	) {
		subdivide();
	}
	
	return true;
}

bool Octree::moveEntry(
	uint32_t entryId,
	glm::vec3 entryPrevMinPos, glm::vec3 entryPrevMaxPos,
	glm::vec3 entryMinPos, glm::vec3 entryMaxPos
) {
	if (!canContain(entryPrevMinPos, entryPrevMaxPos)) {
		return false;
	}
	
	if (children != nullptr) {
		for (int i = 0; i < 8; i++) {
			if (children[i].moveEntry(
				entryId,
				entryPrevMinPos, entryPrevMaxPos,
				entryMinPos, entryMaxPos
			)) {
				return true;
			}
		}
	}
	
	for (auto i = entries.begin(); i != entries.end(); i++) {
		if (i->id == entryId) {
			if (canContain(entryMinPos, entryMaxPos)) {
				i->minPos = entryMinPos;
				i->maxPos = entryMaxPos;
				return true;
			}
			
			entries.erase(i);
			
			auto o = this;
			while (!o->addEntry(entryId, entryMinPos, entryMaxPos)) {
				o = o->parent;
				if (o == nullptr) {
					return false;
				}
			}
			
			return true;
		}
	}
	
	return false;
}

bool Octree::removeEntry(
	uint32_t entryId,
	glm::vec3 entryMinPos, glm::vec3 entryMaxPos
) {
	if (!canContain(entryMinPos, entryMaxPos)) {
		return false;
	}
	
	if (children != nullptr) {
		for (int i = 0; i < 8; i++) {
			if (children[i].removeEntry(entryId, entryMinPos, entryMaxPos)) {
				return true;
			}
		}
	}
	
	for (auto i = entries.begin(); i != entries.end(); i++) {
		if (i->id == entryId) {
			entries.erase(i);
			
			return true;
		}
	}
	
	return false;
}
