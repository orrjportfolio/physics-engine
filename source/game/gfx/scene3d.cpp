#include "scene3d.hpp"

#include <cstddef>
#include <utility>
#include <vector>

namespace Scene3d {
	struct LitProgram : public Program {
		GLint
			uModelMat,
			uViewMat,
			uProjMat,
			uTex,
			uColour;
	};
	
	struct DebugProgram : public Program {
		GLint
			uViewMat,
			uProjMat;
	};
	
	struct DebugPointVert {
		glm::vec3 pos;
		glm::vec3 colour;
	};
	
	struct DebugPointMesh {
		GLuint array, buf;
		size_t numVerts;
	};
	
	struct Object {
		Mesh3d const *mesh;
		Material const *material;
		glm::mat4 mat;
	};
	
	struct DebugObject {
		enum Kind {
			KIND_POINT,
			KIND_LINE,
			KIND_SPHERE,
			KIND_CUBE
		};
		Kind kind;
		glm::vec3 pos;
		union {
			glm::vec3 endPos;
			float radius;
			glm::vec3 halfExtents;
		};
		glm::mat3 rot;
		glm::vec3 colour;
		bool isOnTop;
		float dur;
	};
	
	Cam cam;
	
	static Shader
		litVertShader,
		litFragShader,
		debugPointVertShader,
		debugPointFragShader,
		debugMeshVertShader,
		debugMeshFragShader;
	
	static LitProgram
		litProgram;
	static DebugProgram
		debugPointProgram,
		debugMeshProgram;
	
	static Tex
		whiteTex;
	
	static DebugPointMesh
		debugPointMesh;
	
	static MeshDebug
		debugDiskMesh,
		debugSphereMesh,
		debugCubeMesh;
	
	static std::vector<Object>
		objects;
	
	static std::vector<DebugObject>
		debugObjects;
	
	void init() {
		litVertShader = Shader::load("assets/shaders/lit.vert.glsl", GL_VERTEX_SHADER);
		litFragShader = Shader::load("assets/shaders/lit.frag.glsl", GL_FRAGMENT_SHADER);
		debugPointVertShader = Shader::load("assets/shaders/debugPoint.vert.glsl", GL_VERTEX_SHADER);
		debugPointFragShader = Shader::load("assets/shaders/debugPoint.frag.glsl", GL_FRAGMENT_SHADER);
		debugMeshVertShader = Shader::load("assets/shaders/debugMesh.vert.glsl", GL_VERTEX_SHADER);
		debugMeshFragShader = Shader::load("assets/shaders/debugMesh.frag.glsl", GL_FRAGMENT_SHADER);
		
		Shader const *litShaders[] = {&litVertShader, &litFragShader};
		litProgram = (LitProgram)Program::create(litShaders);
		litProgram.uModelMat = litProgram.uniform("u_modelMat");
		litProgram.uViewMat = litProgram.uniform("u_viewMat");
		litProgram.uProjMat = litProgram.uniform("u_projMat");
		litProgram.uTex = litProgram.uniform("u_tex");
		litProgram.uColour = litProgram.uniform("u_colour");
		
		Shader const *debugPointShaders[] = {&debugPointVertShader, &debugPointFragShader};
		debugPointProgram = (DebugProgram)Program::create(debugPointShaders);
		debugPointProgram.uViewMat = debugPointProgram.uniform("u_viewMat");
		debugPointProgram.uProjMat = debugPointProgram.uniform("u_projMat");
		
		Shader const *debugMeshShaders[] = {&debugMeshVertShader, &debugMeshFragShader};
		debugMeshProgram = (DebugProgram)Program::create(debugMeshShaders);
		debugMeshProgram.uViewMat = debugMeshProgram.uniform("u_viewMat");
		debugMeshProgram.uProjMat = debugMeshProgram.uniform("u_projMat");
		
		auto white = glm::u8vec4(255, 255, 255, 255);
		whiteTex = Tex::create(&white, 1, 1, false, (Tex::FlagMask)0);
		
		glGenVertexArrays(1, &debugPointMesh.array);
		glBindVertexArray(debugPointMesh.array);
		
		glGenBuffers(1, &debugPointMesh.buf);
		glBindBuffer(GL_ARRAY_BUFFER, debugPointMesh.buf);
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DebugPointVert), (void*)offsetof(DebugPointVert, pos));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DebugPointVert), (void*)offsetof(DebugPointVert, colour));
		
		debugDiskMesh = MeshDebug::load("assets/models/debugDisk.obj");
		debugSphereMesh = MeshDebug::load("assets/models/debugSphere.obj");
		debugCubeMesh = MeshDebug::load("assets/models/debugCube.obj");
	}
	
	void draw(int clientW, int clientH, float dt) {
		auto
			viewMat = cam.viewMat(),
			projMat = cam.projMat(clientW / (float)clientH);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glViewport(0, 0, clientW, clientH);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		
		glActiveTexture(GL_TEXTURE0);
		
		glUseProgram(litProgram.handle);
		
		glUniformMatrix4fv(litProgram.uViewMat, 1, GL_FALSE, &viewMat[0][0]);
		glUniformMatrix4fv(litProgram.uProjMat, 1, GL_FALSE, &projMat[0][0]);
		glUniform1i(litProgram.uTex, 0);
		
		for (auto o : objects) {
			auto mKind = o.material->kind;
			if (mKind == Material::KIND_LIT) {
				glBindTexture(GL_TEXTURE_2D, o.material->tex->handle);
			}
			else if (mKind == Material::KIND_LIT_UNTEXED) {
				glBindTexture(GL_TEXTURE_2D, whiteTex.handle);
			}
			
			glUniformMatrix4fv(litProgram.uModelMat, 1, GL_FALSE, &o.mat[0][0]);
			glUniform3fv(litProgram.uColour, 1, &o.material->colour[0]);
			
			o.mesh->draw();
		}
		
		objects.clear();
		
		glDisable(GL_CULL_FACE);
		
		std::vector<DebugPointVert>
			debugPointsTop,
			debugLinesTop,
			debugPoints,
			debugLines;
		
		std::vector<MeshDebug::Instance>
			debugDisksTop,
			debugSpheresTop,
			debugCubesTop,
			debugDisks,
			debugSpheres,
			debugCubes;
		
		for (auto o = debugObjects.begin(); o != debugObjects.end(); o++) {
			if (o->kind == DebugObject::KIND_POINT) {
				(o->isOnTop? debugPointsTop : debugPoints)
					.push_back(DebugPointVert{
						.pos = o->pos,
						.colour = o->colour
					});
			}
			else if (o->kind == DebugObject::KIND_LINE) {
				auto &vec = (o->isOnTop? debugLinesTop : debugLines);
				
				vec.push_back(DebugPointVert{
					.pos = o->pos,
					.colour = o->colour
				});
				vec.push_back(DebugPointVert{
					.pos = o->endPos,
					.colour = o->colour
				});
			}
			else if (o->kind == DebugObject::KIND_SPHERE) {
				(o->isOnTop? debugSpheresTop : debugSpheres)
					.push_back(MeshDebug::Instance{
						.colour = o->colour,
						.mat =
							glm::translate(o->pos) *
							glm::mat4(o->rot) *
							glm::scale(glm::vec3(o->radius))
					});
				
				auto diffToCam = o->pos - cam.pos;
				auto distToCam = glm::length(diffToCam);
				
				if (distToCam > o->radius) {
					auto dirToCam = diffToCam / distToCam;
					
					auto rotAxis = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), dirToCam));
					auto rotAngle = glm::acos(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), dirToCam));
					
					auto clipNorm = -dirToCam;
					auto clipDist = (o->radius * o->radius) / distToCam;
					
					auto clipDistOverRadius = clipDist / o->radius;
					auto clipRadius = o->radius * glm::sqrt(1.0f - (clipDistOverRadius * clipDistOverRadius));
					
					(o->isOnTop? debugDisksTop : debugDisks)
						.push_back(MeshDebug::Instance{
							.colour = o->colour,
							.mat =
								glm::translate(o->pos + (clipNorm * clipDist)) *
								glm::rotate(rotAngle, rotAxis) *
								glm::scale(glm::vec3(clipRadius))
						});
				}
			}
			else if (o->kind == DebugObject::KIND_CUBE) {
				(o->isOnTop? debugCubesTop : debugCubes)
					.push_back(MeshDebug::Instance{
						.colour = o->colour,
						.mat =
							glm::translate(o->pos) *
							glm::mat4(o->rot) *
							glm::scale(o->halfExtents)
					});
			}
			
			o->dur -= dt;
			if (o->dur < 0.0f) {
				std::swap(*o, debugObjects.back());
				debugObjects.resize(debugObjects.size() - 1);
				o--;
			}
		}
		
		auto drawPointMesh = [](std::span<DebugPointVert const> verts, GLenum mode) {
			glBindVertexArray(debugPointMesh.array);
			
			glBindBuffer(GL_ARRAY_BUFFER, debugPointMesh.buf);
			if (verts.size() > debugPointMesh.numVerts) {
				glBufferData(GL_ARRAY_BUFFER, verts.size_bytes(), verts.data(), GL_DYNAMIC_DRAW);
				debugPointMesh.numVerts = verts.size();
			}
			else {
				glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size_bytes(), verts.data());
			}
			
			glDrawArrays(mode, 0, verts.size());
		};
		
		glPointSize(6.0f);
		
		glUseProgram(debugMeshProgram.handle);
		
		glUniformMatrix4fv(debugMeshProgram.uViewMat, 1, GL_FALSE, &viewMat[0][0]);
		glUniformMatrix4fv(debugMeshProgram.uProjMat, 1, GL_FALSE, &projMat[0][0]);
		
		if (!debugDisks.empty()) {
			debugDiskMesh.draw(debugDisks);
			
			debugDisks.clear();
		}
		
		if (!debugSpheres.empty()) {
			debugSphereMesh.draw(debugSpheres);
			
			debugSpheres.clear();
		}
		
		if (!debugCubes.empty()) {
			debugCubeMesh.draw(debugCubes);
			
			debugCubes.clear();
		}
		
		glUseProgram(debugPointProgram.handle);
		
		glUniformMatrix4fv(debugPointProgram.uViewMat, 1, GL_FALSE, &viewMat[0][0]);
		glUniformMatrix4fv(debugPointProgram.uProjMat, 1, GL_FALSE, &projMat[0][0]);
		
		drawPointMesh(debugPoints, GL_POINTS);
		drawPointMesh(debugLines, GL_LINES);
		
		glDisable(GL_DEPTH_TEST);
		
		glUseProgram(debugMeshProgram.handle);
		
		if (!debugDisksTop.empty()) {
			debugDiskMesh.draw(debugDisks);
			
			debugDisksTop.clear();
		}
		
		if (!debugSpheresTop.empty()) {
			debugSphereMesh.draw(debugSpheres);
			
			debugSpheresTop.clear();
		}
		
		if (!debugCubesTop.empty()) {
			debugCubeMesh.draw(debugCubes);
			
			debugCubesTop.clear();
		}
		
		glUseProgram(debugPointProgram.handle);
		
		drawPointMesh(debugPointsTop, GL_POINTS);
		drawPointMesh(debugLinesTop, GL_LINES);
	}
	
	void addObject(Mesh3d const *mesh, Material const *material, glm::mat4 mat) {
		objects.push_back(Object{
			.mesh = mesh,
			.material = material,
			.mat = mat
		});
	}
	
	void addDebugPoint(glm::vec3 pos, glm::vec3 colour, bool isOnTop, float dur) {
		debugObjects.push_back(DebugObject{
			.kind = DebugObject::KIND_POINT,
			.pos = pos,
			.colour = colour,
			.isOnTop = isOnTop,
			.dur = dur
		});
	}
	
	void addDebugLine(glm::vec3 startPos, glm::vec3 endPos, glm::vec3 colour, bool isOnTop, float dur) {
		debugObjects.push_back(DebugObject{
			.kind = DebugObject::KIND_LINE,
			.pos = startPos,
			.endPos = endPos,
			.colour = colour,
			.isOnTop = isOnTop,
			.dur = dur
		});
	}
	
	void addDebugSphere(glm::vec3 pos, float radius, glm::mat3 const &rot, glm::vec3 colour, bool isOnTop, float dur) {
		debugObjects.push_back(DebugObject{
			.kind = DebugObject::KIND_SPHERE,
			.pos = pos,
			.radius = radius,
			.rot = rot,
			.colour = colour,
			.isOnTop = isOnTop,
			.dur = dur
		});
	}
	
	void addDebugCube(glm::vec3 pos, glm::vec3 halfExtents, glm::mat3 const &rot, glm::vec3 colour, bool isOnTop, float dur) {
		debugObjects.push_back(DebugObject{
			.kind = DebugObject::KIND_CUBE,
			.pos = pos,
			.halfExtents = halfExtents,
			.rot = rot,
			.colour = colour,
			.isOnTop = isOnTop,
			.dur = dur
		});
	}
};
