#include "app.hpp"

#include <cassert>

#include <GL/gl3w.h>
#include <SDL2/SDL.h>

#include "entity/entity.hpp"
#include "gfx/scene3d.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl2.h"
#include "game.hpp"

namespace App {
	constexpr float
		TARGET_FPS = 60.0f,
		TARGET_FRAME_DUR = 1.0f / TARGET_FPS;
	
	static SDL_Window *window;
	
	static void init() {
		Scene3d::init();
		Game::init();
	}
	
	static void update(float dt) {
		auto keysHeld = SDL_GetKeyboardState(nullptr);
		static auto prevSpaceHeld = false;
		if (keysHeld[SDL_SCANCODE_SPACE]/* && !prevSpaceHeld*/) {
			for (int i = 0; i < 2; i++) {
				Entity::simulateAll(TARGET_FRAME_DUR / 2.0f);
			}
		}
		prevSpaceHeld = keysHeld[SDL_SCANCODE_SPACE];
		
		Game::update(dt);
		
		int windowW, windowH;
		SDL_GetWindowSize(window, &windowW, &windowH);
		
		Entity::addAllToScene3d();
		
		Scene3d::draw(windowW, windowH, dt);
	}
	
	void run() {
		auto sdlInitResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
		assert(sdlInitResult == 0);
		
		window = SDL_CreateWindow(
			"",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			1280, 720,
			SDL_WINDOW_HIDDEN |
			SDL_WINDOW_OPENGL |
			SDL_WINDOW_RESIZABLE
		);
		assert(window != nullptr);
		
		auto glContext = SDL_GL_CreateContext(window);
		assert(glContext != nullptr);
		
		auto gl3wInitResult = gl3wInit();
		assert(gl3wInitResult == GL3W_OK);
		
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
		SDL_GL_SetSwapInterval(1);
		
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &imguiIo = ImGui::GetIO();
		imguiIo.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		
		ImGui_ImplSDL2_InitForOpenGL(window, glContext);
		ImGui_ImplOpenGL3_Init();
		
		init();
		
		SDL_ShowWindow(window);
		
		float frameDur = 0.0f;
		float dt = 0.0f;
		
		auto startTime = SDL_GetTicks();
		
		bool shouldQuit = false;
		
		while (!shouldQuit) {
			auto currentTime = SDL_GetTicks();
			auto elapsedTime = (currentTime - startTime) / 1000.0f;
			startTime = currentTime;
			
			frameDur += elapsedTime;
			dt += elapsedTime;
			
			if (frameDur >= TARGET_FRAME_DUR) {
				SDL_Event event;
				while (SDL_PollEvent(&event) != 0) {
					if (event.type == SDL_QUIT) {
						shouldQuit = true;
					}
					
					ImGui_ImplSDL2_ProcessEvent(&event);
				}
				
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplSDL2_NewFrame();
				ImGui::NewFrame();
				
				update(dt);
				
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				
				SDL_GL_SwapWindow(window);
				
				frameDur = fmodf(frameDur, TARGET_FRAME_DUR);
				dt = 0.0f;
			}
		}
	}
}
