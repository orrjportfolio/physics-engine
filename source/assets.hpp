#pragma once

#include "gfx.hpp"

static inline GLuint
	surface3dVertShader,
	surface3dFragShader,
	debug3dVertShader,
	debug3dFragShader,
	debugPoint3dVertShader,
	debugPoint3dFragShader;

static inline struct {
	GLuint handle;
	GLint
		uModelMat,
		uViewMat,
		uProjMat,
		uTex,
		uColour;
} surface3dProgram;

static inline struct {
	GLuint handle;
	GLint
		uModelMat,
		uViewMat,
		uProjMat,
		uColour;
} debug3dProgram;

static inline struct {
	GLuint handle;
	GLint
		uViewMat,
		uProjMat;
} debugPoint3dProgram;

static inline Tex whiteTex;

static inline Mesh3d
	sphereMesh,
	planeMesh,
	cubeMesh;

static inline Mesh3d
	diskDebugMesh,
	sphereDebugMesh,
	cubeDebugMesh;

static inline struct {
	GLuint array;
	GLuint vertexBuf;
	size_t numVertices;
} scratchDebugMesh;

static void loadAssets() {
	surface3dVertShader = shaderLoad("assets/shaders/surface3d.vert.glsl", GL_VERTEX_SHADER);
	surface3dFragShader = shaderLoad("assets/shaders/surface3d.frag.glsl", GL_FRAGMENT_SHADER);
	debug3dVertShader = shaderLoad("assets/shaders/debug3d.vert.glsl", GL_VERTEX_SHADER);
	debug3dFragShader = shaderLoad("assets/shaders/debug3d.frag.glsl", GL_FRAGMENT_SHADER);
	debugPoint3dVertShader = shaderLoad("assets/shaders/debugPoint3d.vert.glsl", GL_VERTEX_SHADER);
	debugPoint3dFragShader = shaderLoad("assets/shaders/debugPoint3d.frag.glsl", GL_FRAGMENT_SHADER);
	
	surface3dProgram.handle = programCreate(surface3dVertShader, surface3dFragShader);
	surface3dProgram.uModelMat = glGetUniformLocation(surface3dProgram.handle, "u_modelMat");
	surface3dProgram.uViewMat = glGetUniformLocation(surface3dProgram.handle, "u_viewMat");
	surface3dProgram.uProjMat = glGetUniformLocation(surface3dProgram.handle, "u_projMat");
	surface3dProgram.uTex = glGetUniformLocation(surface3dProgram.handle, "u_tex");
	surface3dProgram.uColour = glGetUniformLocation(surface3dProgram.handle, "u_colour");
	
	debug3dProgram.handle = programCreate(debug3dVertShader, debug3dFragShader);
	debug3dProgram.uModelMat = glGetUniformLocation(debug3dProgram.handle, "u_modelMat");
	debug3dProgram.uViewMat = glGetUniformLocation(debug3dProgram.handle, "u_viewMat");
	debug3dProgram.uProjMat = glGetUniformLocation(debug3dProgram.handle, "u_projMat");
	debug3dProgram.uColour = glGetUniformLocation(debug3dProgram.handle, "u_colour");
	
	debugPoint3dProgram.handle = programCreate(debugPoint3dVertShader, debugPoint3dFragShader);
	debugPoint3dProgram.uViewMat = glGetUniformLocation(debugPoint3dProgram.handle, "u_viewMat");
	debugPoint3dProgram.uProjMat = glGetUniformLocation(debugPoint3dProgram.handle, "u_projMat");
	
	glm::u8vec4 white = glm::u8vec4(255, 255, 255, 255);
	whiteTex = texCreate(&white, 1, 1, false, false);
	
	sphereMesh = mesh3dLoad("assets/models/sphere.obj");
	planeMesh = mesh3dLoad("assets/models/plane.obj");
	cubeMesh = mesh3dLoad("assets/models/cube.obj");
	
	diskDebugMesh = debugMesh3dLoad("assets/models/debug/disk.obj");
	sphereDebugMesh = debugMesh3dLoad("assets/models/debug/sphere.obj");
	cubeDebugMesh = debugMesh3dLoad("assets/models/debug/cube.obj");
	
	glGenVertexArrays(1, &scratchDebugMesh.array);
	glBindVertexArray(scratchDebugMesh.array);
	
	glGenBuffers(1, &scratchDebugMesh.vertexBuf);
	glBindBuffer(GL_ARRAY_BUFFER, scratchDebugMesh.vertexBuf);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 2, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 2, (void*)sizeof(glm::vec3));
	
	scratchDebugMesh.numVertices = 0;
}
