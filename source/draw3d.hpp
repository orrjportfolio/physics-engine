#pragma once

#include <algorithm>
#include <vector>

#include "gfx.hpp"
#include "assets.hpp"

struct Cam {
	glm::vec3 pos;
	float pitch, yaw;
};

static inline Cam cam3d;

static inline glm::mat4
	cam3dViewMat,
	cam3dProjMat;

struct DrawMesh3dCommand {
	Mesh3d const *mesh;
	Tex const *tex;
	glm::vec3 colour;
	glm::mat4 mat;
};

static inline std::vector<DrawMesh3dCommand>
	drawMesh3dCommands;

enum DrawDebug3dCommandKind {
	DRAW_DEBUG_3D_COMMAND_POINT,
	DRAW_DEBUG_3D_COMMAND_LINE,
	DRAW_DEBUG_3D_COMMAND_SPHERE,
	DRAW_DEBUG_3D_COMMAND_CUBE
};

struct DrawDebug3dCommand {
	DrawDebug3dCommandKind kind;
	glm::vec3 pos;
	union {
		glm::vec3 endPos;
		float radius;
		glm::vec3 size;
	};
	glm::mat3 rot;
	glm::vec3 colour;
	bool shouldDrawOnTop;
	float duration;
};

static inline std::vector<DrawDebug3dCommand>
	drawDebug3dCommands;

struct DebugPoint3dVertex {
	glm::vec3 pos;
	glm::vec3 colour;
};

static void draw3d(int clientW, int clientH, float dt) {
	glUseProgram(surface3dProgram.handle);
	
	cam3dViewMat =
		glm::rotate(cam3d.pitch, glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(cam3d.yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(-cam3d.pos);
	cam3dProjMat =
		glm::perspective(70.0f, clientW / (float)clientH, 0.01f, 300.0f);
	
	glUniformMatrix4fv(surface3dProgram.uViewMat, 1, GL_FALSE, &cam3dViewMat[0][0]);
	glUniformMatrix4fv(surface3dProgram.uProjMat, 1, GL_FALSE, &cam3dProjMat[0][0]);
	glUniform1i(surface3dProgram.uTex, 0);
	
	glActiveTexture(GL_TEXTURE0);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	for (DrawMesh3dCommand const &command : drawMesh3dCommands) {
		glBindTexture(GL_TEXTURE_2D, command.tex->handle);
		
		glUniformMatrix4fv(surface3dProgram.uModelMat, 1, GL_FALSE, &command.mat[0][0]);
		glUniform3f(surface3dProgram.uColour, command.colour.r, command.colour.g, command.colour.b);
		
		mesh3dDraw(command.mesh);
	}
	
	drawMesh3dCommands.clear();
	
	glUseProgram(debug3dProgram.handle);
	
	glUniformMatrix4fv(debug3dProgram.uViewMat, 1, GL_FALSE, &cam3dViewMat[0][0]);
	glUniformMatrix4fv(debug3dProgram.uProjMat, 1, GL_FALSE, &cam3dProjMat[0][0]);
	
	glDisable(GL_CULL_FACE);
	
	std::vector<DebugPoint3dVertex>
		pointVertices,
		lineVertices,
		pointVerticesTop,
		lineVerticesTop;
	
	for (size_t i = 0; i < drawDebug3dCommands.size(); i++) {
		DrawDebug3dCommand &command = drawDebug3dCommands[i];
		
		glUniform3f(debug3dProgram.uColour, command.colour.r, command.colour.g, command.colour.b);
		
		if (command.shouldDrawOnTop) {
			glDisable(GL_DEPTH_TEST);
		}
		else {
			glEnable(GL_DEPTH_TEST);
		}
		
		if (command.kind == DRAW_DEBUG_3D_COMMAND_POINT) {
			(command.shouldDrawOnTop? pointVerticesTop : pointVertices).push_back(DebugPoint3dVertex{
				.pos = command.pos,
				.colour = command.colour
			});
		}
		else if (command.kind == DRAW_DEBUG_3D_COMMAND_LINE) {
			(command.shouldDrawOnTop? lineVerticesTop : lineVertices).push_back(DebugPoint3dVertex{
				.pos = command.pos,
				.colour = command.colour
			});
			(command.shouldDrawOnTop? lineVerticesTop : lineVertices).push_back(DebugPoint3dVertex{
				.pos = command.endPos,
				.colour = command.colour
			});
		}
		else if (command.kind == DRAW_DEBUG_3D_COMMAND_SPHERE) {
			glm::mat4 mat = glm::translate(command.pos) * glm::mat4(command.rot) * glm::scale(glm::vec3(command.radius));
			glUniformMatrix4fv(debug3dProgram.uModelMat, 1, GL_FALSE, &mat[0][0]);
			
			debugMesh3dDraw(&sphereDebugMesh);
			
			glm::vec3 diffToCamera = command.pos - cam3d.pos;
			float distToCamera = glm::length(diffToCamera);
			
			if (distToCamera >= command.radius) {
				glm::vec3 dirToCamera = glm::normalize(diffToCamera);
				
				glm::vec3 rotAxis = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), dirToCamera));
				float rotAngle = glm::acos(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), dirToCamera));
				
				glm::vec3 clipNorm = -dirToCamera;
				float clipDist = (command.radius * command.radius) / distToCamera;
				
				float clipDistOverRadius = clipDist / command.radius;
				float clipRadius = command.radius * glm::sqrt(1.0f - (clipDistOverRadius * clipDistOverRadius));
				
				mat = glm::translate(command.pos + (clipNorm * clipDist)) * glm::rotate(rotAngle, rotAxis) * glm::scale(glm::vec3(clipRadius));
				glUniformMatrix4fv(debug3dProgram.uModelMat, 1, GL_FALSE, &mat[0][0]);
				
				debugMesh3dDraw(&diskDebugMesh);
			}
		}
		else if (command.kind == DRAW_DEBUG_3D_COMMAND_CUBE) {
			glm::mat4 mat = glm::translate(command.pos) * glm::mat4(command.rot) * glm::scale(command.size);
			glUniformMatrix4fv(debug3dProgram.uModelMat, 1, GL_FALSE, &mat[0][0]);
			
			debugMesh3dDraw(&cubeDebugMesh);
		}
		
		if (command.duration <= 0.0f) {
			std::swap(drawDebug3dCommands[i], drawDebug3dCommands.back());
			drawDebug3dCommands.pop_back();
			i--;
		}
		else {
			command.duration -= dt;
		}
	}
	
	glUseProgram(debugPoint3dProgram.handle);
	
	glUniformMatrix4fv(debugPoint3dProgram.uViewMat, 1, GL_FALSE, &cam3dViewMat[0][0]);
	glUniformMatrix4fv(debugPoint3dProgram.uProjMat, 1, GL_FALSE, &cam3dProjMat[0][0]);
	
	glPointSize(8.0f);
	
	glBindVertexArray(scratchDebugMesh.array);
	glBindBuffer(GL_ARRAY_BUFFER, scratchDebugMesh.vertexBuf);
	
	auto const f = [&](GLenum mode, std::span<DebugPoint3dVertex const> vertices) {
		if (vertices.size() < scratchDebugMesh.numVertices) {
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size_bytes(), vertices.data());
		}
		else {
			glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_DYNAMIC_DRAW);
			scratchDebugMesh.numVertices = vertices.size();
		}
		glDrawArrays(mode, 0, vertices.size());
	};
	
	glEnable(GL_DEPTH_TEST);
	
	f(GL_POINTS, pointVertices);
	f(GL_LINES, lineVertices);
	
	glDisable(GL_DEPTH_TEST);
	
	f(GL_POINTS, pointVerticesTop);
	f(GL_LINES, lineVerticesTop);
}

static void queueDrawMesh3d(
	Mesh3d const *mesh,
	Tex const *tex,
	glm::vec3 colour,
	glm::vec3 pos,
	glm::vec3 scale,
	glm::mat3 rot
) {
	drawMesh3dCommands.push_back(DrawMesh3dCommand{
		.mesh = mesh,
		.tex = tex,
		.colour = colour,
		.mat = glm::translate(pos) * glm::mat4(rot) * glm::scale(scale)
	});
}

static void queueDrawDebugPoint(glm::vec3 pos, glm::vec3 colour, bool shouldDrawOnTop, float duration) {
	drawDebug3dCommands.push_back(DrawDebug3dCommand{
		.kind = DRAW_DEBUG_3D_COMMAND_POINT,
		.pos = pos,
		.colour = colour,
		.shouldDrawOnTop = shouldDrawOnTop,
		.duration = duration
	});
}

static void queueDrawDebugLine(glm::vec3 startPos, glm::vec3 endPos, glm::vec3 colour, bool shouldDrawOnTop, float duration) {
	drawDebug3dCommands.push_back(DrawDebug3dCommand{
		.kind = DRAW_DEBUG_3D_COMMAND_LINE,
		.pos = startPos,
		.endPos = endPos,
		.colour = colour,
		.shouldDrawOnTop = shouldDrawOnTop,
		.duration = duration
	});
}

static void queueDrawDebugSphere(glm::vec3 pos, float radius, glm::mat3 rot, glm::vec3 colour, bool shouldDrawOnTop, float duration) {
	drawDebug3dCommands.push_back(DrawDebug3dCommand{
		.kind = DRAW_DEBUG_3D_COMMAND_SPHERE,
		.pos = pos,
		.radius = radius,
		.rot = rot,
		.colour = colour,
		.shouldDrawOnTop = shouldDrawOnTop,
		.duration = duration
	});
}

static void queueDrawDebugCube(glm::vec3 pos, glm::vec3 size, glm::mat3 rot, glm::vec3 colour, bool shouldDrawOnTop, float duration) {
	drawDebug3dCommands.push_back(DrawDebug3dCommand{
		.kind = DRAW_DEBUG_3D_COMMAND_CUBE,
		.size = size,
		.rot = rot,
		.colour = colour,
		.shouldDrawOnTop = shouldDrawOnTop,
		.duration = duration
	});
}
