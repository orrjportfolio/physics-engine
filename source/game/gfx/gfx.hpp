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
	
	GLint uniform(char const *name) {
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
	
	static Tex create(void const *dataRgba32, int w, int h, bool alpha, FlagMask flags);
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
	
	void draw() {
		glBindVertexArray(array);
		
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, (void*)0);
	}
};
