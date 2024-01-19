#pragma once

#include <cassert>
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

namespace Gfx3d {
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
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
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
	
	struct Vertex {
		glm::vec3 pos;
		glm::i16vec3 norm;
		glm::vec2 uv;
	};
	
	struct Mesh {
		GLuint array;
		union {
			struct { GLuint vertexBuf, indexBuf; };
			GLuint bufs[2];
		};
		size_t numIndices;
		GLenum primitiveKind;
	};
	
	static Mesh meshCreate(std::span<Vertex const> vertices, std::span<uint16_t const> indices, GLenum primitiveKind) {
		Mesh mesh;
		
		glGenVertexArrays(1, &mesh.array);
		glBindVertexArray(mesh.array);
		
		glGenBuffers(2, mesh.bufs);
		
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuf);
		glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
		glVertexAttribPointer(1, 3, GL_SHORT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, norm));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuf);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), GL_STATIC_DRAW);
		
		mesh.numIndices = indices.size();
		
		mesh.primitiveKind = primitiveKind;
		
		return mesh;
	}
	
	static Mesh meshLoad(char const *path) {
		std::vector<glm::vec3> poses;
		std::vector<glm::vec3> norms;
		std::vector<glm::vec2> uvs;
		
		std::vector<Vertex> vertices;
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
						vertices.push_back(Vertex{
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
		
		return meshCreate(vertices, indices, GL_TRIANGLES);
	}
	
	static void meshDraw(Mesh const *mesh) {
		glBindVertexArray(mesh->array);
		
		glDrawElements(mesh->primitiveKind, mesh->numIndices, GL_UNSIGNED_SHORT, (void*)0);
	}
	
	static inline GLuint
		surface3dVertShader,
		surface3dFragShader;
	
	static inline struct {
		GLuint handle;
		GLint
			uModelMat,
			uViewMat,
			uProjMat,
			uCamPos,
			uTex,
			uColour,
			uSmoothness;
	} surface3dProgram;
	
	static inline Tex whiteTex;
	
	static inline Mesh
		sphereMesh,
		planeMesh,
		cubeMesh;
	
	struct Cam {
		glm::vec3 pos;
		float pitch, yaw;
	};
	
	static inline Cam cam;
	
	struct DrawMeshCommand {
		Mesh const *mesh;
		Tex const *tex;
		glm::vec3 colour;
		glm::mat4 mat;
	};
	
	static inline std::vector<DrawMeshCommand>
		drawMeshCommands;
	
	static void init() {
		surface3dVertShader = shaderLoad("assets/shaders/surface3d.vert.glsl", GL_VERTEX_SHADER);
		surface3dFragShader = shaderLoad("assets/shaders/surface3d.frag.glsl", GL_FRAGMENT_SHADER);
		
		surface3dProgram.handle = programCreate(surface3dVertShader, surface3dFragShader);
		surface3dProgram.uModelMat = glGetUniformLocation(surface3dProgram.handle, "u_modelMat");
		surface3dProgram.uViewMat = glGetUniformLocation(surface3dProgram.handle, "u_viewMat");
		surface3dProgram.uProjMat = glGetUniformLocation(surface3dProgram.handle, "u_projMat");
		surface3dProgram.uCamPos = glGetUniformLocation(surface3dProgram.handle, "u_camPos");
		surface3dProgram.uTex = glGetUniformLocation(surface3dProgram.handle, "u_tex");
		surface3dProgram.uColour = glGetUniformLocation(surface3dProgram.handle, "u_colour");
		surface3dProgram.uSmoothness = glGetUniformLocation(surface3dProgram.handle, "u_smoothness");
		
		glm::u8vec4 white = glm::u8vec4(255, 255, 255, 255);
		whiteTex = texCreate(&white, 1, 1, false, false);
		
		sphereMesh = meshLoad("assets/models/sphere.obj");
		planeMesh = meshLoad("assets/models/plane.obj");
		cubeMesh = meshLoad("assets/models/cube.obj");
	}
	
	static void draw(int clientW, int clientH) {
		//glClearColor(1.0f/3.0f, 2.0f/3.0f, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glViewport(0, 0, clientW, clientH);
		
		glActiveTexture(GL_TEXTURE0);
		
		glUseProgram(surface3dProgram.handle);
		
		glm::mat4 viewMat =
			glm::rotate(cam.pitch, glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::rotate(cam.yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::translate(-cam.pos);
		glm::mat4 projMat =
			glm::perspective(70.0f, clientW / (float)clientH, 0.01f, 300.0f);
		
		glUniformMatrix4fv(surface3dProgram.uViewMat, 1, GL_FALSE, &viewMat[0][0]);
		glUniformMatrix4fv(surface3dProgram.uProjMat, 1, GL_FALSE, &projMat[0][0]);
		glUniform3f(surface3dProgram.uCamPos, cam.pos.x, cam.pos.y, cam.pos.z);
		glUniform1i(surface3dProgram.uTex, 0);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		
		for (DrawMeshCommand const &command : drawMeshCommands) {
			glBindTexture(GL_TEXTURE_2D, command.tex->handle);
			
			glUniformMatrix4fv(surface3dProgram.uModelMat, 1, GL_FALSE, &command.mat[0][0]);
			glUniform3f(surface3dProgram.uColour, command.colour.r, command.colour.g, command.colour.b);
			
			meshDraw(command.mesh);
		}
		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		
		drawMeshCommands.clear();
	}
	
	static void queueDrawMesh(
		Mesh const *mesh,
		Tex const *tex,
		glm::vec3 colour,
		glm::vec3 pos,
		glm::vec3 scale,
		glm::mat3 orientation
	) {
		drawMeshCommands.push_back(DrawMeshCommand{
			.mesh = mesh,
			.tex = tex,
			.colour = colour,
			.mat = glm::translate(pos) * glm::scale(scale) * glm::mat4(orientation)
		});
	}
}
