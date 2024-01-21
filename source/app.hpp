
#include <cassert>
#include <chrono>
#include <iostream>

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"
#include "assets.hpp"
#include "draw3d.hpp"
#include "game.hpp"

static inline SDL_Window *window;
static inline SDL_GLContext glContext;

static glm::vec3 spheres[1000];

static void appInit() {
	loadAssets();
	
	gameInit();
	
	for (int i = 0; i < 1000; i++) {
		spheres[i] = glm::vec3(
			(rand() / (float)RAND_MAX) * 100.0f,
			(rand() / (float)RAND_MAX) * 100.0f,
			(rand() / (float)RAND_MAX) * 100.0f
		);
	}
}

static void appUpdate(float dt) {
	int windowW, windowH;
	SDL_GetWindowSize(window, &windowW, &windowH);
	
	gameUpdate(dt);
	
	static float time;
	time += dt;
	
	Uint8 const *keysHeld = SDL_GetKeyboardState(nullptr);
	glm::ivec2 mouseVel;
	Uint32 mouseButtons = SDL_GetRelativeMouseState(&mouseVel.x, &mouseVel.y);
	
	if (mouseButtons & SDL_BUTTON_RMASK) {
		SDL_SetRelativeMouseMode(SDL_TRUE);
		
		cam3d.pitch += mouseVel.y * 0.5f * dt;
		cam3d.yaw += mouseVel.x * 0.5f * dt;
		
		glm::mat3 mat =
			glm::rotate(-cam3d.yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(-cam3d.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 right = mat * glm::vec3(1.0f, 0.0f, 00.0f);
		glm::vec3 up = mat * glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 back = mat * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		
		if (keysHeld[SDL_SCANCODE_A]) { cam3d.pos -= right * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_D]) { cam3d.pos += right * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_Q]) { cam3d.pos -= up * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_E]) { cam3d.pos += up * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_W]) { cam3d.pos -= back * 10.0f * dt; }
		if (keysHeld[SDL_SCANCODE_S]) { cam3d.pos += back * 10.0f * dt; }
	}
	else {
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
	
	static bool prevHeldSpace = false;
	bool heldSpace = keysHeld[SDL_SCANCODE_SPACE];
	for (int i = 0; i < 1000; i++) {
		glm::vec3 pos = spheres[i];
		
		if (heldSpace && !prevHeldSpace) {
			queueDrawDebugSphere(pos, 0.55f, glm::rotate(time, glm::vec3(1.0f, 1.0f, 1.0f)), glm::vec3(0.0f, 1.0f, 0.0f), false, 2.0f);
			//queueDrawDebugCube(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f), glm::rotate(time, glm::vec3(1.0f, 1.0f, 1.0f)), glm::vec3(0.0f, 1.0f, 0.0f), true, 1.0f);
			queueDrawDebugLine(glm::vec3(0.0f, 0.0f, 0.0f), pos, glm::vec3(1.0f, 1.0f, 0.0f), true, 2.0f);
			queueDrawDebugPoint(pos, glm::vec3(1.0f, 0.0f, 0.0f), true, 2.0f);
		}
		queueDrawMesh3d(
			&sphereMesh,
			&whiteTex,
			glm::vec3(1.0f, 1.0f, 1.0f),
			pos,
			glm::vec3(0.5f),
			glm::identity<glm::mat3>()
		);
	}
	prevHeldSpace = heldSpace;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glViewport(0, 0, windowW, windowH);
	
	auto start = std::chrono::system_clock::now();
	
	draw3d(windowW, windowH, dt);
	
	auto end = std::chrono::system_clock::now();
	std::cout << (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0f) << " ms\n";
}

static void appRun() {
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
	
	appInit();
	
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
			
			appUpdate(frameTime / 1000.0f);
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			
			SDL_GL_SwapWindow(window);
			
			timer = fmod(timer, targetFrameDur);
			frameTime = 0.0f;
		}
	}
}
