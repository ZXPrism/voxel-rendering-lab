#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array_buffer.h>
#include <logger.h>
#include <orbit_camera.h>
#include <shader.h>
#include <texture.h>
#include <vertex_array.h>
#include <world.h>

#include <chrono>
#include <memory>

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 960;
constexpr glm::vec3 CAMERA_POS{ 0.01f, 80.0f, 0.0f };

struct {
	float _delta_time = 0.0f;

	SDL_Window *_window;
	SDL_GLContextState *_context;

	std::unique_ptr<vox::Shader> _shader;

	vox::OrbitCamera _camera;

	std::unique_ptr<vox::Texture> _grass_block;
	std::unique_ptr<vox::Texture> _dirt_block;
	std::unique_ptr<vox::Texture> _stone_block;

	vox::World _world;

	bool init() {
		if (!SDL_Init(SDL_INIT_VIDEO)) {
			SPDLOG_ERROR("Couldn't initialize SDL: {}", SDL_GetError());
			return false;
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		_window = SDL_CreateWindow("voxel-rendering-lab", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
		if (_window == nullptr) {
			SPDLOG_ERROR("Couldn't create window: {}\n", SDL_GetError());
			return false;
		}

		_context = SDL_GL_CreateContext(_window);
		if (_context == nullptr) {
			SPDLOG_ERROR("Couldn't create OpenGL context: {}\n", SDL_GetError());
			SDL_DestroyWindow(_window);
			return false;
		}

		if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress)) == 0) {
			SPDLOG_ERROR("Couldn't init OpenGL!\n");
			SDL_GL_DestroyContext(_context);
			SDL_DestroyWindow(_window);
			return false;
		}

		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		SDL_GL_SetSwapInterval(0);  // no vsync

		return true;
	}

	void prep() {
		_shader = std::make_unique<vox::Shader>("assets/shader/cube.vert", "assets/shader/cube.frag");
		_grass_block = std::make_unique<vox::Texture>("assets/texture/grass_block.png");
		_dirt_block = std::make_unique<vox::Texture>("assets/texture/dirt_block.png");
		_stone_block = std::make_unique<vox::Texture>("assets/texture/stone_block.png");

		_grass_block->bind_texture(0);
		_dirt_block->bind_texture(1);
		_stone_block->bind_texture(2);

		_camera.look_at(CAMERA_POS, glm::vec3(0, 0, 0));
		_camera.set_perspective(
		    glm::radians(90.0f),
		    static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
		    0.1f, 1000.0f);

		_world.generate();
	}

	void update() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto &shader = _shader;
		shader->use_program();
		shader->set_uniform("uProjectionView", _camera.get_vp_matrix());
		shader->set_uniform("uCameraPos", CAMERA_POS);
		shader->set_uniform("uBlockTextureGrass", 0);
		shader->set_uniform("uBlockTextureDirt", 1);
		shader->set_uniform("uBlockTextureStone", 2);

		_world.render();
		_camera.process_input(_delta_time);

		SDL_GL_SwapWindow(_window);
	}

	void shutdown() const {
		SDL_GL_DestroyContext(_context);
	}
} global_state;

SDL_AppResult SDL_AppInit([[maybe_unused]] void **appstate, [[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
	SDL_SetAppMetadata("vox", "1.0", "com.example.renderer-clear");

	if (!global_state.init()) {
		return SDL_APP_FAILURE;
	}
	global_state.prep();

	SPDLOG_INFO("OpenGL Version: {}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));
	SPDLOG_INFO("GLSL Version: {}", reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
	SPDLOG_INFO("Renderer: {}", reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
	SPDLOG_INFO("Vendor: {}", reinterpret_cast<const char *>(glGetString(GL_VENDOR)));

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent([[maybe_unused]] void *appstate, SDL_Event *event) {
	switch (event->type) {
	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;
	case SDL_EVENT_MOUSE_WHEEL:
		global_state._camera.process_input_2(global_state._delta_time, event->wheel.y);
		break;
	default:
		break;
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate([[maybe_unused]] void *appstate) {
	auto t = std::chrono::steady_clock::now();
	static std::optional<decltype(t)> prev_t;
	if (prev_t.has_value()) {
		global_state._delta_time = std::chrono::duration_cast<std::chrono::microseconds>(t - *prev_t).count() / 1000000.0f;
	}
	prev_t = t;

	global_state.update();

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit([[maybe_unused]] void *appstate, [[maybe_unused]] SDL_AppResult result) {
	global_state.shutdown();
}
