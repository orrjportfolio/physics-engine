
#include <cassert>
#include <iostream>

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL.h>

#include "gfx3d.hpp"

namespace App {
	static inline SDL_Window *window;
	static inline SDL_GLContext glContext;
	
	static void init() {
		Gfx3d::init();
	}
	
	static void update() {
		int windowW, windowwH;
		SDL_GetWindowSize(window, &windowW, &windowwH);
		
		float s = glm::sin(SDL_GetTicks() * 0.001f);
		
		Gfx3d::queueDrawPlane(glm::vec2(2.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(-0.5f, s, 0.0f), glm::rotate(s, glm::vec3(0.5f, 1.0f, 0.5f)));
		Gfx3d::queueDrawCuboid(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.5f, -s, 0.0f), glm::rotate(s, glm::vec3(0.5f, 1.0f, 0.5f)));
		
		Gfx3d::draw(windowW, windowwH);
	}
	
	static void run() {
		int sdlInitResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
		assert(sdlInitResult == 0);
		
		window = SDL_CreateWindow(
			"",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			1280, 720,
			SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE
		);
		assert(window != nullptr);
		
		glContext = SDL_GL_CreateContext(window);
		assert(glContext != nullptr);
		
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
		SDL_GL_SetSwapInterval(1);
		
		int gl3wInitResult = gl3wInit();
		assert(gl3wInitResult == GL3W_OK);
		
		#ifndef NDEBUG
			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei len, GLchar const *message, void const*) {
				if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) { return; }
				
				char const *sourceStr =
					source == GL_DEBUG_SOURCE_API? "API" :
					source == GL_DEBUG_SOURCE_WINDOW_SYSTEM? "Window System" :
					source == GL_DEBUG_SOURCE_SHADER_COMPILER? "Shader Compiler" :
					source == GL_DEBUG_SOURCE_THIRD_PARTY? "Third Party" :
					source == GL_DEBUG_SOURCE_APPLICATION? "Application" :
					"Other";
				
				char const *typeStr =
					type == GL_DEBUG_TYPE_ERROR? "Error" :
					type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR? "Deprecated Behaviour" :
					type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR? "Undefined Behaviour" :
					type == GL_DEBUG_TYPE_PORTABILITY? "Portability Concern" :
					type == GL_DEBUG_TYPE_PERFORMANCE? "Performance Concern" :
					type == GL_DEBUG_TYPE_MARKER? "Marker" :
					"Other";
				
				char const *severityStr =
					severity == GL_DEBUG_SEVERITY_NOTIFICATION? "Notification" :
					severity == GL_DEBUG_SEVERITY_LOW? "Low Severity" :
					severity == GL_DEBUG_SEVERITY_MEDIUM? "Medium Severity" :
					"High Severity";
				
				std::cout << '[' << sourceStr << "] [" << typeStr << "] [" << severityStr << "] " << message << std::endl;
			}, nullptr);
		#endif
		
		init();
		
		SDL_ShowWindow(window);
		
		while (!SDL_HasEvent(SDL_QUIT)) {
			SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
			SDL_PumpEvents();
			
			update();
			
			SDL_GL_SwapWindow(window);
		}
	}
}
