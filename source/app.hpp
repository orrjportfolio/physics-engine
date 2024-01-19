
#include <cassert>
#include <iostream>

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"
#include "gfx3d.hpp"
#include "game.hpp"

namespace App {
	static inline SDL_Window *window;
	static inline SDL_GLContext glContext;
	
	static void init() {
		Gfx3d::init();
		
		Game::init();
	}
	
	static void update(float dt) {
		int windowW, windowwH;
		SDL_GetWindowSize(window, &windowW, &windowwH);
		
		Game::update(dt);
		
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
		
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui_ImplSDL2_InitForOpenGL(window, glContext);
		ImGui_ImplOpenGL3_Init();
		
		init();
		
		SDL_ShowWindow(window);
		
		float const targetFps = 60.0f;
		float const targetFrameDur = 1000.0f / targetFps;
		
		float timer = 0.0f;
		float frameTime = 0.0f;
		
		Uint32 startTime = SDL_GetTicks();
		
		bool shouldQuit = false;
		
		while (!shouldQuit) {
			Uint32 time = SDL_GetTicks();
			Uint32 elapsedTime = time - startTime;
			
			timer += elapsedTime;
			frameTime += elapsedTime;
			
			startTime = time;
			
			if (timer >= targetFrameDur) {
				SDL_Event e;
				while (SDL_PollEvent(&e) != 0) {
					if (e.type == SDL_QUIT) {
						shouldQuit = true;
					}
					
					ImGui_ImplSDL2_ProcessEvent(&e);
				}
				
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplSDL2_NewFrame();
				ImGui::NewFrame();
				
				update(frameTime / 1000.0f);
				
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				
				SDL_GL_SwapWindow(window);
				
				timer = fmod(timer, targetFrameDur);
				frameTime = 0.0f;
			}
		}
	}
}
