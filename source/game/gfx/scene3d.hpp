#pragma once

#include <GL/gl3w.h>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

#include "gfx.hpp"

struct Material {
	enum Kind {
		KIND_LIT,
		KIND_LIT_UNTEXED
	};
	
	Kind kind;
	Tex const *tex;
	glm::vec3 colour;
};

namespace Scene3d {
	struct Cam {
		glm::mat4 viewMat;
		
		glm::mat4 projMat(float aspect) {
			return
				glm::perspective(glm::radians(70.0f), aspect, 0.03f, 300.0f);
		}
	};
	
	extern Cam cam;
	
	void init();
	
	void draw(int clientW, int clientH, float dt);
	
	void addObject(Mesh3d const *mesh, Material const *material, glm::mat4 mat);
	
	void addDebugPoint(glm::vec3 pos, glm::vec3 colour, bool isOnTop, float dur);
	
	void addDebugLine(glm::vec3 startPos, glm::vec3 endPos, glm::vec3 colour, bool isOnTop, float dur);
	
	void addDebugSphere(glm::vec3 pos, float radius, glm::mat3 const &rot, glm::vec3 colour, bool isOnTop, float dur);
	
	void addDebugCube(glm::vec3 pos, glm::vec3 halfExtents, glm::mat3 const &rot, glm::vec3 colour, bool isOnTop, float dur);
	
}
