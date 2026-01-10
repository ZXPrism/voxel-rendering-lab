#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array_buffer.h>
#include <logger.h>
#include <shader.h>
#include <texture.h>
#include <vertex_array.h>
#include <world.h>

#include <memory>
#include <vector>

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 960;
constexpr glm::vec3 CAMERA_POS{ 0.01f, 80.0f, 0.0f };

struct {
	SDL_Window *_window;
	SDL_GLContextState *_context;

	std::unique_ptr<vox::Shader> _shader;

	glm::mat4 _pv;

	std::unique_ptr<vox::Texture> _grass_block;
	std::unique_ptr<vox::Texture> _dirt_block;
	std::unique_ptr<vox::Texture> _stone_block;

	vox::World _world;
} state;

SDL_AppResult SDL_AppInit([[maybe_unused]] void **appstate, [[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
	SDL_SetAppMetadata("vox", "1.0", "com.example.renderer-clear");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SPDLOG_ERROR("Couldn't initialize SDL: {}", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	state._window = SDL_CreateWindow("voxel-rendering-lab", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	if (state._window == nullptr) {
		SPDLOG_ERROR("Couldn't create window: {}\n", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	state._context = SDL_GL_CreateContext(state._window);
	if (state._context == nullptr) {
		SPDLOG_ERROR("Couldn't create OpenGL context: {}\n", SDL_GetError());
		SDL_DestroyWindow(state._window);
		return SDL_APP_FAILURE;
	}

	if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)) == 0) {
		SPDLOG_ERROR("Couldn't init OpenGL!\n");
		SDL_GL_DestroyContext(state._context);
		SDL_DestroyWindow(state._window);
		return SDL_APP_FAILURE;
	}

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	SDL_GL_SetSwapInterval(0);  // no vsync

	SPDLOG_INFO("OpenGL Version: {}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));
	SPDLOG_INFO("GLSL Version: {}", reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
	SPDLOG_INFO("Renderer: {}", reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
	SPDLOG_INFO("Vendor: {}", reinterpret_cast<const char *>(glGetString(GL_VENDOR)));

	state._shader = std::make_unique<vox::Shader>("assets/shader/cube.vert", "assets/shader/cube.frag");
	state._grass_block = std::make_unique<vox::Texture>("assets/texture/grass_block.png");
	state._dirt_block = std::make_unique<vox::Texture>("assets/texture/dirt_block.png");
	state._stone_block = std::make_unique<vox::Texture>("assets/texture/stone_block.png");

	state._grass_block->bind_texture(0);
	state._dirt_block->bind_texture(1);
	state._stone_block->bind_texture(2);

	glm::mat4 view = glm::lookAt(
	    CAMERA_POS,
	    glm::vec3(0, 0, 0),
	    glm::vec3(0, 1, 0));
	glm::mat4 proj = glm::perspective(
	    glm::radians(90.0f),
	    static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
	    0.1f, 1000.0f);
	state._pv = proj * view;

	state._world.generate();

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent([[maybe_unused]] void *appstate, SDL_Event *event) {
	switch (event->type) {
	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate([[maybe_unused]] void *appstate) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto &shader = state._shader;
	shader->use_program();
	shader->set_uniform("uProjectionView", state._pv);
	shader->set_uniform("uCameraPos", CAMERA_POS);
	shader->set_uniform("uBlockTextureGrass", 0);
	shader->set_uniform("uBlockTextureDirt", 1);
	shader->set_uniform("uBlockTextureStone", 2);

	state._world.render();

	SDL_GL_SwapWindow(state._window);
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit([[maybe_unused]] void *appstate, [[maybe_unused]] SDL_AppResult result) {
	SDL_GL_DestroyContext(state._context);
}
