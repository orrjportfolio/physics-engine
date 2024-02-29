#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include "../util.hpp"

struct Shader {
	GLuint handle;
	
	static Shader create(char const *source, GLenum kind);
	
	static Shader load(char const *path, GLenum kind);
};

struct Program {
	GLuint handle;
	
	static Program create(std::span<Shader const*> shaders);
	
	GLint uniform(char const *name) const {
		return glGetUniformLocation(handle, name);
	}
};

struct Tex {
	enum FlagMask {
		FLAG_MIPMAP = 0x1,
		FLAG_FILTER = 0x2
	};
	
	GLuint handle;
	int w, h;
	
	static Tex create(void const *dataRgba32, int w, int h, bool alpha, int flags);
	
	static Tex load(char const *path, int flags);
};

struct Mesh {
	GLuint array;
	GLuint vertBuf, idxBuf;
	size_t numIndices;
};

struct Vert3d {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 uv;
};

struct Mesh3d : public Mesh {
	static Mesh3d create(
		std::span<Vert3d const> verts,
		std::span<uint16_t const> idxs
	);
	
	static Mesh3d load(char const *path);
	
	void draw() const {
		glBindVertexArray(array);
		
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, (void*)0);
	}
};

struct MeshDebug : public Mesh {
	struct Instance {
		glm::vec3 colour;
		glm::mat4 mat;
	};
	
	GLuint instanceBuf;
	
	size_t numInstances;
	
	static MeshDebug create(
		std::span<glm::vec3 const> verts,
		std::span<uint16_t const> idxs
	);
	
	static MeshDebug load(char const *path);
	
	void draw(std::span<Instance const> instances) {
		glBindVertexArray(array);
		
		glBindBuffer(GL_ARRAY_BUFFER, instanceBuf);
		if (instances.size() > numInstances) {
			glBufferData(GL_ARRAY_BUFFER, instances.size_bytes(), instances.data(), GL_DYNAMIC_DRAW);
			
			numInstances = instances.size();
		}
		else {
			glBufferSubData(GL_ARRAY_BUFFER, 0, instances.size_bytes(), instances.data());
		}
		
		glDrawElementsInstanced(GL_LINES, numIndices, GL_UNSIGNED_SHORT, (void*)0, instances.size());
	}
};
