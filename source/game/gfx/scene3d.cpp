#include "scene3d.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

namespace Scene3d {
	Cam cam;
	
	static Shader
		litVertShader,
		litFragShader;
	
	static Program
		litProgram;
	
	static Mesh3d
		cubeMesh;
	
	void init() {
		litVertShader = Shader::load("assets/shaders/lit.vert.glsl", GL_VERTEX_SHADER);
		litFragShader = Shader::load("assets/shaders/lit.frag.glsl", GL_FRAGMENT_SHADER);
		
		Shader const *litProgramShaders[] = {&litVertShader, &litFragShader};
		litProgram = Program::create(litProgramShaders);
		
		cubeMesh = Mesh3d::load("assets/models/sphere.obj");
		
		cam.pos = glm::vec3(0.0f, 0.0f, 3.0f);
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
		
		auto time = SDL_GetTicks() / 1000.0f;
		
		auto modelMat =
			glm::rotate(time, glm::vec3(1.0f, 1.0f, 1.0f));
		auto viewMat =
			glm::rotate(cam.pitch, glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::rotate(cam.yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::translate(-cam.pos);
		auto projMat =
			glm::perspective(glm::radians(70.0f), clientW / (float)clientH, 0.03f, 300.0f);
		
		auto
			uModelMat = litProgram.uniform("u_modelMat"),
			uViewMat = litProgram.uniform("u_viewMat"),
			uProjMat = litProgram.uniform("u_projMat");
		
		glUseProgram(litProgram.handle);
		
		glUniformMatrix4fv(uModelMat, 1, GL_FALSE, &modelMat[0][0]);
		glUniformMatrix4fv(uViewMat, 1, GL_FALSE, &viewMat[0][0]);
		glUniformMatrix4fv(uProjMat, 1, GL_FALSE, &projMat[0][0]);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		
		cubeMesh.draw();
		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
	}
};
