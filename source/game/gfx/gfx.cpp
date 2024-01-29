#include "gfx.hpp"

#include <cstdlib>
#include <functional>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include <vector>

Shader Shader::create(char const *source, GLenum kind) {
	auto handle = glCreateShader(kind);
	
	glShaderSource(handle, 1, &source, nullptr);
	
	glCompileShader(handle);
	
	#ifndef NDEBUG
		GLint compiled;
		glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLint infoLogLen;
			glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLen);
			
			char *infoLog = (char*)malloc(infoLogLen + 1);
			glGetShaderInfoLog(handle, infoLogLen, &infoLogLen, infoLog);
			infoLog[infoLogLen] = '\0';
			
			std::cerr << infoLog << '\n';
			__debugbreak();
		}
	#endif
	
	return Shader{.handle = handle};
}

Shader Shader::load(char const *path, GLenum kind) {
	auto source = Util::loadStr(path, nullptr);
	
	auto r = Shader::create(source, kind);
	
	return r;
}

Program Program::create(std::span<Shader const*> shaders) {
	auto handle = glCreateProgram();
	
	for (auto shader : shaders) {
		glAttachShader(handle, shader->handle);
	}
	
	glLinkProgram(handle);
	
	#ifndef NDEBUG
		GLint linked;
		glGetProgramiv(handle, GL_LINK_STATUS, &linked);
		if (!linked) {
			GLint infoLogLen;
			glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &infoLogLen);
			
			char *infoLog = (char*)malloc(infoLogLen + 1);
			glGetProgramInfoLog(handle, infoLogLen, &infoLogLen, infoLog);
			infoLog[infoLogLen] = '\0';
			
			std::cerr << infoLog << '\n';
			__debugbreak();
		}
	#endif
	
	return Program{.handle = handle};
}

Tex Tex::create(void const *dataRgba32, int w, int h, bool alpha, FlagMask flags) {
	GLuint handle;
	glGenTextures(1, &handle);
	
	glBindTexture(GL_TEXTURE_2D, handle);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		(flags & FLAG_FILTER)?
			(flags & FLAG_MIPMAP)?
				GL_LINEAR_MIPMAP_LINEAR :
				GL_NEAREST_MIPMAP_LINEAR :
			(flags & FLAG_MIPMAP)?
				GL_LINEAR :
				GL_NEAREST
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		(flags & FLAG_FILTER)?
			GL_LINEAR :
			GL_NEAREST
	);
	
	glTexImage2D(
		GL_TEXTURE_2D, 0,
		alpha? GL_RGBA : GL_RGB,
		w, h,
		0,
		GL_RGBA, GL_UNSIGNED_BYTE,
		dataRgba32
	);
	
	if (flags & FLAG_MIPMAP) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	
	return Tex{
		.handle = handle,
		.w = w, .h = h
	};
}

Mesh3d Mesh3d::create(
	std::span<Vert3d const> verts,
	std::span<uint16_t const> idxs
) {
	GLuint array;
	glGenVertexArrays(1, &array);
	
	GLuint bufs[2];
	glGenBuffers(2, bufs);
	auto
		vertBuf = bufs[0],
		idxBuf = bufs[1];
	
	glBindVertexArray(array);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
	glBufferData(GL_ARRAY_BUFFER, verts.size_bytes(), verts.data(), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert3d), (void*)offsetof(Vert3d, pos));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vert3d), (void*)offsetof(Vert3d, norm));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vert3d), (void*)offsetof(Vert3d, uv));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxs.size_bytes(), idxs.data(), GL_STATIC_DRAW);
	
	return (Mesh3d)Mesh{
		.array = array,
		.vertBuf = vertBuf, .idxBuf = idxBuf,
		.numIndices = idxs.size()
	};
}

Mesh3d Mesh3d::load(char const *path) {
	auto s = Util::loadStr(path, nullptr);
	
	std::vector<glm::vec3>
		poses, norms;
	std::vector<glm::vec2>
		uvs;
	
	std::vector<Vert3d>
		verts;
	std::unordered_map<
		glm::u16vec3, uint16_t,
		decltype([](glm::u16vec3 v) {
			return std::hash<std::string_view>()(
				std::string_view((char*)&v, sizeof(v))
			);
		})
	>
		vertByPosNormAndUv;
	
	std::vector<uint16_t>
		idxs;
	
	auto it = s;
	while (*it != '\0') {
		if (*it == 'v') {
			it++;
			
			if (*it == 'n') {
				it++;
				
				glm::vec3 norm;
				norm.x = strtod(it, &it);
				norm.y = strtod(it, &it);
				norm.z = strtod(it, &it);
				
				norms.push_back(norm);
			}
			else if (*it == 't') {
				it++;
				
				glm::vec2 uv;
				uv.x = strtod(it, &it);
				uv.y = strtod(it, &it);
				
				uvs.push_back(uv);
			}
			else {
				glm::vec3 pos;
				pos.x = strtod(it, &it);
				pos.y = strtod(it, &it);
				pos.z = strtod(it, &it);
				
				poses.push_back(pos);
			}
		}
		else if (*it == 'f') {
			it++;
			
			for (int i = 0; i < 3; i++) {
				auto pos = (uint16_t)(strtoul(it, &it, 10) - 1);
				it++;
				auto uv = (uint16_t)(strtoul(it, &it, 10) - 1);
				it++;
				auto norm = (uint16_t)(strtoul(it, &it, 10) - 1);
				
				auto vert = vertByPosNormAndUv.find(glm::u16vec3(pos, norm, uv));
				if (vert != vertByPosNormAndUv.end()) {
					idxs.push_back(vert->second);
				}
				else {
					idxs.push_back(verts.size());
					verts.push_back(Vert3d{
						.pos = poses[pos],
						.norm = norms[norm],
						.uv = uvs[uv]
					});
				}
			}
		}
		
		while (*it++ != '\n') { }
	}
	
	free(s);
	
	return Mesh3d::create(verts, idxs);
}
