#include "scene3d.hpp"

#include <vector>

#include <SDL2/SDL.h>

namespace Scene3d {
	struct LitProgram : public Program {
		GLint
			uModelMat,
			uViewMat,
			uProjMat,
			uTex,
			uColour;
	};
	
	struct Object {
		Mesh3d const *mesh;
		Material const *material;
		glm::mat4 mat;
	};
	
	Cam cam;
	
	static Shader
		litVertShader,
		litFragShader;
	
	static LitProgram
		litProgram;
	
	static Tex
		whiteTex;
	
	static std::vector<Object>
		objects;
	
	void init() {
		litVertShader = Shader::load("assets/shaders/lit.vert.glsl", GL_VERTEX_SHADER);
		litFragShader = Shader::load("assets/shaders/lit.frag.glsl", GL_FRAGMENT_SHADER);
		
		Shader const *litShaders[] = {&litVertShader, &litFragShader};
		litProgram = (LitProgram)Program::create(litShaders);
		litProgram.uModelMat = litProgram.uniform("u_modelMat");
		litProgram.uViewMat = litProgram.uniform("u_viewMat");
		litProgram.uProjMat = litProgram.uniform("u_projMat");
		litProgram.uTex = litProgram.uniform("u_tex");
		litProgram.uColour = litProgram.uniform("u_colour");
		
		auto white = glm::u8vec4(255, 255, 255, 255);
		whiteTex = Tex::create(&white, 1, 1, false, (Tex::FlagMask)0);
	}
	
	void draw(int clientW, int clientH) {
		auto keysHeld = SDL_GetKeyboardState(nullptr);
		int mouseRelX, mouseRelY;
		auto mouseButtonsHeld = SDL_GetRelativeMouseState(&mouseRelX, &mouseRelY);
		if (mouseButtonsHeld & SDL_BUTTON_RMASK) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			
			auto dt = 1.0f / 60.0f;
			
			cam.pitch += mouseRelY * 0.5f * dt;
			cam.yaw += mouseRelX * 0.5f * dt;
			
			auto mat = glm::mat3(
				glm::rotate(-cam.yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
				glm::rotate(-cam.pitch, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			
			auto right = mat * glm::vec3(1.0f, 0.0f, 0.0f);
			auto up = mat * glm::vec3(0.0f, 1.0f, 0.0f);
			auto back = mat * glm::vec3(0.0f, 0.0f, 1.0f);
			
			if (keysHeld[SDL_SCANCODE_A]) { cam.pos -= right * 10.0f * dt; }
			if (keysHeld[SDL_SCANCODE_D]) { cam.pos += right * 10.0f * dt; }
			if (keysHeld[SDL_SCANCODE_Q]) { cam.pos -= up * 10.0f * dt; }
			if (keysHeld[SDL_SCANCODE_E]) { cam.pos += up * 10.0f * dt; }
			if (keysHeld[SDL_SCANCODE_W]) { cam.pos -= back * 10.0f * dt; }
			if (keysHeld[SDL_SCANCODE_S]) { cam.pos += back * 10.0f * dt; }
			
		}
		else {
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glViewport(0, 0, clientW, clientH);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		
		glActiveTexture(GL_TEXTURE0);
		
		glUseProgram(litProgram.handle);
		
		auto
			viewMat = cam.viewMat(),
			projMat = cam.projMat(clientW / (float)clientH);
		
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
		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		
		objects.clear();
	}
	
	void addObject(Mesh3d const *mesh, Material const *material, glm::mat4 mat) {
		objects.push_back(Object{
			.mesh = mesh,
			.material = material,
			.mat = mat
		});
	}
};
