/*#pragma once

#include <span>
#include <vector>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "gfx.hpp"
#include "gfx3d.hpp"
#include "util.hpp"

struct Gfx3dDebugMesh {
	GLuint array;
	union {
		struct { GLuint vertexBuf, indexBuf; };
		GLuint bufs[2];
	};
	size_t numIndices;
};

static Gfx3dDebugMesh gfx3dDebugMeshCreate(std::span<glm::vec3 const> vertices, std::span<uint16_t const> indices) {
	Gfx3dDebugMesh mesh;
	
	glGenVertexArrays(1, &mesh.array);
	glBindVertexArray(mesh.array);
	
	glGenBuffers(2, mesh.bufs);
	
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuf);
	glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), GL_STATIC_DRAW);
	
	mesh.numIndices = indices.size();
	
	return mesh;
}

static Gfx3dDebugMesh gfx3dDebugMeshLoad(char const *path) {
	std::vector<glm::vec3> vertices;
	std::vector<uint16_t> indices;
	
	char *sb = strLoad(path, nullptr);
	
	char *s = sb;
	while (*s != '\0') {
		if (*s == 'v') {
			float x = strtod(s, &s);
			float y = strtod(s, &s);
			float z = strtod(s, &s);
			vertices.push_back(glm::vec3(x, y, z));
		}
		else if (*s == 'l') {
			for (int i = 0; i < 2; i++) {
				uint16_t index = (uint16_t)strtoul(s, &s, 10);
				indices.push_back(index);
			}
		}
		
		while (*s++ != '\n') { }
	}
	
	free(sb);
	
	return gfx3dDebugMeshCreate(vertices, indices);
}

static void gfx3dDebugMeshDraw(Gfx3dDebugMesh const *mesh) {
	glBindVertexArray(mesh->array);
	
	glDrawElements(GL_LINES, mesh->numIndices, GL_UNSIGNED_SHORT, (void*)0);
}

static inline GLuint
	gfx3dDebugVertShader,
	gfx3dDebugFragShader;

static inline struct {
	GLuint handle;
	GLint uColour;
} gfx3dDebugProgram;

enum Gfx3dDebugDrawCommandKind {
	GFX_3D_DEBUG_DRAW_COMMAND_SPHERE,
	GFX_3D_DEBUG_DRAW_COMMAND_CUBE,
	GFX_3D_DEBUG_DRAW_COMMAND_LINE
};

struct Gfx3dDebugDrawCommand {
	Gfx3dDebugDrawCommandKind kind;
	bool onTop;
	glm::vec3 pos;
	union {
		float radius;
		glm::vec3 size;
		glm::vec3 endPos;
	};
};

static inline std::vector<Gfx3dDebugDrawCommand>
	gfx3dDebugDrawCommands;

static void gfx3dDebugInit() {
	gfx3dDebugVertShader = gfxShaderLoad("shaders/3d.vert.glsl", GL_VERTEX_SHADER);
	gfx3dDebugFragShader = gfxShaderLoad("shaders/3d.frag.glsl", GL_FRAGMENT_SHADER);
	
	gfx3dDebugProgram.handle = gfxProgramCreate(gfx3dDebugVertShader, gfx3dDebugFragShader);
	gfx3dDebugProgram.uColour = glGetUniformLocation(gfx3dDebugProgram.handle, "u_colour");
}

static void gfx3dDebugDraw(int clientW, int clientH) {
	glUseProgram(gfx3dDebugProgram.handle);
	
	glm::mat4 viewMat =
		glm::rotate(gfx3dCam.pitch, glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(gfx3dCam.yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(-gfx3dCam.pos);
	glm::mat4 projMat =
		glm::perspective(70.0f, clientW / (float)clientH, 0.01f, 300.0f);
	
	glUniformMatrix4fv(gfx3dProgram.uViewMat, 1, GL_FALSE, &viewMat[0][0]);
	glUniformMatrix4fv(gfx3dProgram.uProjMat, 1, GL_FALSE, &projMat[0][0]);
	glUniform3f(gfx3dProgram.uCamPos, gfx3dCam.pos.x, gfx3dCam.pos.y, gfx3dCam.pos.z);
}
*/