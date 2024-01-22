#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

#include <GL/gl3w.h>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

#include <stb/stb_image.h>

#include "util.hpp"

static GLuint shaderCreate(char const *source, GLenum kind) {
	GLuint shader = glCreateShader(kind);
	
	glShaderSource(shader, 1, &source, nullptr);
	
	glCompileShader(shader);
	
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	#ifndef NDEBUG
		if (!compiled) {
			GLint infoLogLen;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
			
			char *infoLog = (char*)malloc(infoLogLen + 1);
			glGetShaderInfoLog(shader, infoLogLen, nullptr, infoLog);
			
			__debugbreak();
		}
	#endif
	
	return shader;
}

static GLuint shaderLoad(char const *path, GLenum kind) {
	char *source = strLoad(path, nullptr);
	
	GLuint shader = shaderCreate(source, kind);
	
	free(source);
	
	return shader;
}

static GLuint programCreate(GLuint shader1, GLuint shader2) {
	GLuint program = glCreateProgram();
	
	glAttachShader(program, shader1);
	glAttachShader(program, shader2);
	
	glLinkProgram(program);
	
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	#ifndef NDEBUG
		if (!linked) {
			GLint infoLogLen;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
			
			char *infoLog = (char*)malloc(infoLogLen + 1);
			glGetProgramInfoLog(program, infoLogLen, nullptr, infoLog);
			
			__debugbreak();
		}
	#endif
	
	return program;
}

struct Tex {
	GLuint handle;
	int w, h;
};

static Tex texCreate(void const *data, int w, int h, bool alpha, bool mipmap) {
	Tex tex;
	
	glGenTextures(1, &tex.handle);
	
	glBindTexture(GL_TEXTURE_2D, tex.handle);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexImage2D(GL_TEXTURE_2D, 0, alpha? GL_RGBA : GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
	if (mipmap) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	
	tex.w = w;
	tex.h = h;
	
	return tex;
}

static Tex texLoad(char const *path, bool mipmap) {
	int w, h, c;
	void *data = stbi_load(path, &w, &h, &c, 4);
	assert(data != nullptr);
	
	Tex tex = texCreate(data, w, h, c % 2 == 0, mipmap);
	
	stbi_image_free(data);
	
	return tex;
}


struct Vertex3d {
	glm::vec3 pos;
	glm::i16vec3 norm;
	glm::vec2 uv;
};

struct Mesh3d {
	GLuint array;
	union {
		struct { GLuint vertexBuf, indexBuf; };
		GLuint bufs[2];
	};
	size_t numIndices;
};

static Mesh3d mesh3dCreate(std::span<Vertex3d const> vertices, std::span<uint16_t const> indices) {
	Mesh3d mesh;
	
	glGenVertexArrays(1, &mesh.array);
	glBindVertexArray(mesh.array);
	
	glGenBuffers(2, mesh.bufs);
	
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuf);
	glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3d), (void*)offsetof(Vertex3d, pos));
	glVertexAttribPointer(1, 3, GL_SHORT, GL_TRUE, sizeof(Vertex3d), (void*)offsetof(Vertex3d, norm));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3d), (void*)offsetof(Vertex3d, uv));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), GL_STATIC_DRAW);
	
	mesh.numIndices = indices.size();
	
	return mesh;
}

static Mesh3d mesh3dLoad(char const *path) {
	std::vector<glm::vec3> poses;
	std::vector<glm::vec3> norms;
	std::vector<glm::vec2> uvs;
	
	std::vector<Vertex3d> vertices;
	std::vector<uint16_t> indices;
	std::unordered_map<glm::u16vec3, uint16_t, decltype([](glm::u16vec3 v) {
		return std::hash<std::string_view>{}(
			std::string_view((char*)&v, sizeof(v))
		);
	})> indexMap;
	
	char *sb = strLoad(path, nullptr);
	char *s = sb;
	
	while (*s != '\0') {
		if (*s == 'v') {
			s++;
			char c = *s++;
			if (c == 'n') {
				float x = strtof(s, &s);
				float y = strtof(s, &s);
				float z = strtof(s, &s);
				norms.push_back(glm::vec3(x, y, z));
			}
			else if (c == 't') {
				float x = strtof(s, &s);
				float y = strtof(s, &s);
				uvs.push_back(glm::vec2(x, y));
			}
			else {
				float x = strtof(s, &s);
				float y = strtof(s, &s);
				float z = strtof(s, &s);
				poses.push_back(glm::vec3(x, y, z));
			}
		}
		else if (*s == 'f') {
			for (int i = 0; i < 3; i++) {
				glm::u16vec3 key;
				key.x = (uint16_t)(strtoul(++s, &s, 10) - 1);
				key.y = (uint16_t)(strtoul(++s, &s, 10) - 1);
				key.z = (uint16_t)(strtoul(++s, &s, 10) - 1);
				
				uint16_t index;
				
				auto it = indexMap.find(key);
				if (it != indexMap.end()) {
					index = it->second;
				}
				else {
					index = vertices.size();
					vertices.push_back(Vertex3d{
						.pos = poses[key.x],
						.norm = glm::i16vec3(norms[key.z] * (float)INT16_MAX),
						.uv = uvs[key.y]
					});
					indexMap.insert({key, index});
				}
				
				indices.push_back(index);
			}
		}
		
		while (*s++ != '\n');
	}
	
	free(sb);
	
	return mesh3dCreate(vertices, indices);
}

static void mesh3dDraw(Mesh3d const *mesh) {
	glBindVertexArray(mesh->array);
	
	glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_SHORT, (void*)0);
}

static Mesh3d debugMesh3dCreate(std::span<glm::vec3 const> vertices, std::span<uint16_t> indices) {
	Mesh3d mesh;
	
	glGenVertexArrays(1, &mesh.array);
	glBindVertexArray(mesh.array);
	
	glGenBuffers(2, mesh.bufs);
	
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuf);
	glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), GL_STATIC_DRAW);
	
	mesh.numIndices = indices.size();
	
	return mesh;
}

static Mesh3d debugMesh3dLoad(char const *path) {
	std::vector<glm::vec3> vertices;
	std::vector<uint16_t> indices;
	
	char *sb = strLoad(path, nullptr);
	
	char *s = sb;
	while (*s != '\0') {
		if (*s == 'v') {
			s++;
			
			float x = strtod(s, &s);
			float y = strtod(s, &s);
			float z = strtod(s, &s);
			vertices.push_back(glm::vec3(x, y, z));
		}
		else if (*s == 'l') {
			s++;
			
			for (int i = 0; i < 2; i++) {
				unsigned long index = strtoul(s, &s, 10);
				indices.push_back((uint16_t)(index - 1));
			}
		}
		
		while (*s++ != '\n') { }
	}
	
	free(sb);
	
	return debugMesh3dCreate(vertices, indices);
}

static void debugMesh3dDraw(Mesh3d const *mesh) {
	glBindVertexArray(mesh->array);
	
	glDrawElements(GL_LINES, mesh->numIndices, GL_UNSIGNED_SHORT, (void*)0);
}


struct DebugPoint3dVertex {
	glm::vec3 pos;
	glm::vec3 colour;
};

