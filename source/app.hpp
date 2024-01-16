
#include <cassert>

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL.h>

#include "gfx.hpp"

static inline SDL_Window *window;
static inline SDL_GLContext glContext;

static inline GLuint vshp, fshp, sh;

static inline Gfx::Mesh3D test;

static void init() {
	vshp = Gfx::shaderPartLoad("assets/shaders/basic3d.vert.glsl", GL_VERTEX_SHADER);
	fshp = Gfx::shaderPartLoad("assets/shaders/basic3d.frag.glsl", GL_FRAGMENT_SHADER);
	sh = Gfx::shaderCreate(vshp, fshp);
	
	Gfx::Vertex3D vertices[] = {
		Gfx::Vertex3D{.pos = glm::vec3(-0.5f, 0.5f, -0.5f), .norm = glm::i16vec3(0, 0, INT16_MIN), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(0.5f, 0.5f, -0.5f), .norm = glm::i16vec3(0, 0, INT16_MIN), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(0.5f, -0.5f, -0.5f), .norm = glm::i16vec3(0, 0, INT16_MIN), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(-0.5f, -0.5f, -0.5f), .norm = glm::i16vec3(0, 0, INT16_MIN), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		
		Gfx::Vertex3D{.pos = glm::vec3(-0.5f, -0.5f, 0.5f), .norm = glm::i16vec3(0, 0, INT16_MAX), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(0.5f, -0.5f, 0.5f), .norm = glm::i16vec3(0, 0, INT16_MAX), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(0.5f, 0.5f, 0.5f), .norm = glm::i16vec3(0, 0, INT16_MAX), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(-0.5f, 0.5f, 0.5f), .norm = glm::i16vec3(0, 0, INT16_MAX), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		
		Gfx::Vertex3D{.pos = glm::vec3(-0.5f, -0.5f, -0.5f), .norm = glm::i16vec3(0, INT16_MIN, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(0.5f, -0.5f, -0.5f), .norm = glm::i16vec3(0, INT16_MIN, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(0.5f, -0.5f, 0.5f), .norm = glm::i16vec3(0, INT16_MIN, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(-0.5f, -0.5f, 0.5f), .norm = glm::i16vec3(0, INT16_MIN, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		
		Gfx::Vertex3D{.pos = glm::vec3(-0.5f, 0.5f, 0.5f), .norm = glm::i16vec3(0, INT16_MAX, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(0.5f, 0.5f, 0.5f), .norm = glm::i16vec3(0, INT16_MAX, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(0.5f, 0.5f, -0.5f), .norm = glm::i16vec3(0, INT16_MAX, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(-0.5f, 0.5f, -0.5f), .norm = glm::i16vec3(0, INT16_MAX, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		
		Gfx::Vertex3D{.pos = glm::vec3(-0.5f, -0.5f, 0.5f), .norm = glm::i16vec3(INT16_MIN, 0, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(-0.5f, 0.5f, 0.5f), .norm = glm::i16vec3(INT16_MIN, 0, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(-0.5f, 0.5f, -0.5f), .norm = glm::i16vec3(INT16_MIN, 0, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(-0.5f, -0.5f, -0.5f), .norm = glm::i16vec3(INT16_MIN, 0, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		
		Gfx::Vertex3D{.pos = glm::vec3(0.5f, -0.5f, -0.5f), .norm = glm::i16vec3(INT16_MAX, 0, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(0.5f, 0.5f, -0.5f), .norm = glm::i16vec3(INT16_MAX, 0, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(0.5f, 0.5f, 0.5f), .norm = glm::i16vec3(INT16_MAX, 0, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
		Gfx::Vertex3D{.pos = glm::vec3(0.5f, -0.5f, 0.5f), .norm = glm::i16vec3(INT16_MAX, 0, 0), .colour = glm::u8vec4(255, 255, 255, 255), .uv = glm::vec2(0.0f, 0.0f)},
	};
	uint16_t indices[] = {
		0, 1, 2,
		0, 2, 3,
		
		4, 5, 6,
		4, 6, 7,
		
		8, 9, 10,
		8, 10, 11,
		
		12, 13, 14,
		12, 14, 15,
		
		16, 17, 18,
		16, 18, 19,
		
		20, 21, 22,
		20, 22, 23
	};
	test = Gfx::mesh3dCreate(vertices, indices, GL_TRIANGLES);
}

static void update() {
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	
	glClearColor(1.0f/3.0f, 2.0f/3.0f, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(sh);
	
	GLint uProjMat = glGetUniformLocation(sh, "u_projMat");
	GLint uViewMat = glGetUniformLocation(sh, "u_viewMat");
	GLint uModelMat = glGetUniformLocation(sh, "u_modelMat");
	glm::mat4 projMat =
		glm::perspective(70.0f, w / (float)h, 0.01f, 100.0f);
	glm::mat4 viewMat =
		glm::translate(glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 modelMat =
		glm::rotate(glm::radians((float)SDL_GetTicks() * 0.05f), glm::vec3(0.5f, 1.0f, 0.5f));
	
	glUniformMatrix4fv(uProjMat, 1, GL_FALSE, &projMat[0][0]);
	glUniformMatrix4fv(uViewMat, 1, GL_FALSE, &viewMat[0][0]);
	glUniformMatrix4fv(uModelMat, 1, GL_FALSE, &modelMat[0][0]);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	Gfx::mesh3dDraw(&test);
}

static void appRun() {
	int sdlInitResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
	assert(sdlInitResult == 0);
	
	window = SDL_CreateWindow(
		"",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1280, 720,
		SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN
	);
	assert(window != nullptr);
	
	glContext = SDL_GL_CreateContext(window);
	assert(glContext != nullptr);
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
	SDL_GL_SetSwapInterval(1);
	
	int gl3wInitResult = gl3wInit();
	assert(gl3wInitResult == GL3W_OK);
	
	init();
	
	SDL_ShowWindow(window);
	
	while (!SDL_HasEvent(SDL_QUIT)) {
		SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
		SDL_PumpEvents();
		
		update();
		
		SDL_GL_SwapWindow(window);
	}
}
