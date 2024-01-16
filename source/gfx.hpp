#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <span>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

#include <stb/stb_image.h>

#include "util.hpp"

namespace Gfx {
	struct Tex {
		GLuint handle;
		int w, h;
	};
	
	static Tex texCreate(void const *data, int w, int h, bool alpha, bool mipmap) {
		Tex tex;
		
		glGenTextures(1, &tex.handle);
		
		glBindTexture(GL_TEXTURE_2D, tex.handle);
		
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		glTexImage2D(GL_TEXTURE_2D, 0, alpha? GL_RGBA : GL_RGB, w, h, 0, GL_RGBA, GL_BYTE, data);
		
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
	
	static GLuint shaderPartCreate(char const *source, GLenum kind) {
		GLuint part = glCreateShader(kind);
		
		glShaderSource(part, 1, &source, nullptr);
		
		glCompileShader(part);
		
		GLint compiled;
		glGetShaderiv(part, GL_COMPILE_STATUS, &compiled);
		#ifndef NDEBUG
			if (!compiled) {
				GLint infoLogLen;
				glGetShaderiv(part, GL_INFO_LOG_LENGTH, &infoLogLen);
				
				char *infoLog = (char*)malloc(infoLogLen + 1);
				glGetShaderInfoLog(part, infoLogLen, nullptr, infoLog);
				
				__debugbreak();
			}
		#endif
		
		return part;
	}
	
	static GLuint shaderPartLoad(char const *path, GLenum kind) {
		char *source = strLoad(path, nullptr);
		
		GLuint part = shaderPartCreate(source, kind);
		
		free(source);
		
		return part;
	}
	
	static GLuint shaderCreate(GLuint part1, GLuint part2) {
		GLuint shader = glCreateProgram();
		
		glAttachShader(shader, part1);
		glAttachShader(shader, part2);
		
		glLinkProgram(shader);
		
		GLint linked;
		glGetProgramiv(shader, GL_LINK_STATUS, &linked);
		#ifndef NDEBUG
			if (!linked) {
				GLint infoLogLen;
				glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
				
				char *infoLog = (char*)malloc(infoLogLen + 1);
				glGetProgramInfoLog(shader, infoLogLen, nullptr, infoLog);
				
				__debugbreak();
			}
		#endif
		
		return shader;
	}
	
	struct Vertex2D {
		glm::vec2 pos;
		glm::vec3 colour;
		glm::vec2 uv;
	};
	
	struct Vertex3D {
		glm::vec3 pos;
		glm::i16vec3 norm;
		glm::u8vec4 colour;
		glm::vec2 uv;
	};
	
	struct Mesh3D {
		GLuint array;
		union {
			struct { GLuint vertexBuf, indexBuf; };
			GLuint bufs[2];
		};
		size_t numIndices;
		GLenum primitiveKind;
	};
	
	static Mesh3D mesh3dCreate(std::span<Vertex3D const> vertices, std::span<uint16_t const> indices, GLenum primitiveKind) {
		Mesh3D mesh;
		
		glGenVertexArrays(1, &mesh.array);
		glBindVertexArray(mesh.array);
		
		glGenBuffers(2, mesh.bufs);
		
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuf);
		glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, pos));
		glVertexAttribPointer(1, 3, GL_SHORT, GL_TRUE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, norm));
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, colour));
		glVertexAttribPointer(3, 5, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, uv));
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuf);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), GL_STATIC_DRAW);
		
		mesh.numIndices = indices.size();
		
		mesh.primitiveKind = primitiveKind;
		
		return mesh;
	}
	
	static void mesh3dDraw(Mesh3D const *mesh) {
		glBindVertexArray(mesh->array);
		
		glDrawElements(mesh->primitiveKind, mesh->numIndices, GL_UNSIGNED_SHORT, (void*)0);
	}
}
