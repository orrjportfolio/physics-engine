#pragma once

#include <glm/glm.hpp>

#include "gfx/scene3d.hpp"
#include "entity/entity.hpp"

namespace Map {
	struct Object {
		glm::vec3 pos;
		glm::vec3 halfSize;
		glm::vec3 rot;
		Material *material;
		PhysicsMaterial physMaterial;
	};
	
	extern Object objects[60];
	
	void init();
	
}
