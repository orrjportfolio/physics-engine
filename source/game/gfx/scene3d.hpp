#pragma once

#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "gfx.hpp"

namespace Scene3d {
	struct Cam {
		glm::vec3 pos;
		float pitch, yaw;
	};
	
	extern Cam cam;
	
	void init();
	
	void draw(int clientW, int clientH);
}
