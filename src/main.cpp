#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array_buffer.h>
#include <logger.h>
#include <shader.h>
#include <texture.h>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

#include <memory>
#include <vector>

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 960;
constexpr int WORLD_SIZE_LENGTH = 128;
constexpr glm::vec3 CAMERA_POS{ 0.01f, 80.0f, 0.0f };

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Instance {
	glm::vec3 translation;
	int texture;
};

struct {
	SDL_Window *_window;
	SDL_GLContextState *_context;

	std::unique_ptr<vox::Shader> _shader;
	GLuint _VAO;
	std::unique_ptr<vox::ArrayBuffer> _voxel_data;
	std::unique_ptr<vox::ArrayBuffer> _voxel_index_data;
	std::unique_ptr<vox::ArrayBuffer> _instance_data;
	glm::mat4 _pv;

	int _voxel_cnt = 0;
	std::vector<std::vector<int>> _world_data;  // [x][z] = height

	std::unique_ptr<vox::Texture> _grass_block;
	std::unique_ptr<vox::Texture> _dirt_block;
	std::unique_ptr<vox::Texture> _stone_block;
} state;

static const Vertex cube_vertices[] = {
	// +X
	{ .pos = { 0.5f, -0.5f, -0.5f }, .normal = { 1, 0, 0 }, .uv = { 0, 1 } },
	{ .pos = { 0.5f, 0.5f, -0.5f }, .normal = { 1, 0, 0 }, .uv = { 0, 0 } },
	{ .pos = { 0.5f, 0.5f, 0.5f }, .normal = { 1, 0, 0 }, .uv = { 1, 0 } },
	{ .pos = { 0.5f, -0.5f, 0.5f }, .normal = { 1, 0, 0 }, .uv = { 1, 1 } },
	// -X
	{ .pos = { -0.5f, -0.5f, 0.5f }, .normal = { -1, 0, 0 }, .uv = { 0, 1 } },
	{ .pos = { -0.5f, 0.5f, 0.5f }, .normal = { -1, 0, 0 }, .uv = { 0, 0 } },
	{ .pos = { -0.5f, 0.5f, -0.5f }, .normal = { -1, 0, 0 }, .uv = { 1, 0 } },
	{ .pos = { -0.5f, -0.5f, -0.5f }, .normal = { -1, 0, 0 }, .uv = { 1, 1 } },
	// +Y
	{ .pos = { -0.5f, 0.5f, -0.5f }, .normal = { 0, 1, 0 }, .uv = { 0, 0 } },
	{ .pos = { -0.5f, 0.5f, 0.5f }, .normal = { 0, 1, 0 }, .uv = { 0, 1 } },
	{ .pos = { 0.5f, 0.5f, 0.5f }, .normal = { 0, 1, 0 }, .uv = { 1, 1 } },
	{ .pos = { 0.5f, 0.5f, -0.5f }, .normal = { 0, 1, 0 }, .uv = { 1, 0 } },
	// -Y
	{ .pos = { -0.5f, -0.5f, 0.5f }, .normal = { 0, -1, 0 }, .uv = { 0, 0 } },
	{ .pos = { -0.5f, -0.5f, -0.5f }, .normal = { 0, -1, 0 }, .uv = { 0, 1 } },
	{ .pos = { 0.5f, -0.5f, -0.5f }, .normal = { 0, -1, 0 }, .uv = { 1, 1 } },
	{ .pos = { 0.5f, -0.5f, 0.5f }, .normal = { 0, -1, 0 }, .uv = { 1, 0 } },
	// +Z
	{ .pos = { -0.5f, -0.5f, 0.5f }, .normal = { 0, 0, 1 }, .uv = { 1, 1 } },
	{ .pos = { 0.5f, -0.5f, 0.5f }, .normal = { 0, 0, 1 }, .uv = { 0, 1 } },
	{ .pos = { 0.5f, 0.5f, 0.5f }, .normal = { 0, 0, 1 }, .uv = { 0, 0 } },
	{ .pos = { -0.5f, 0.5f, 0.5f }, .normal = { 0, 0, 1 }, .uv = { 1, 0 } },
	// -Z
	{ .pos = { 0.5f, -0.5f, -0.5f }, .normal = { 0, 0, -1 }, .uv = { 1, 1 } },
	{ .pos = { -0.5f, -0.5f, -0.5f }, .normal = { 0, 0, -1 }, .uv = { 0, 1 } },
	{ .pos = { -0.5f, 0.5f, -0.5f }, .normal = { 0, 0, -1 }, .uv = { 0, 0 } },
	{ .pos = { 0.5f, 0.5f, -0.5f }, .normal = { 0, 0, -1 }, .uv = { 1, 0 } },
};

static const uint32_t cube_indices[] = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4,
	8, 9, 10, 10, 11, 8,
	12, 13, 14, 14, 15, 12,
	16, 17, 18, 18, 19, 16,
	20, 21, 22, 22, 23, 20
};

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

	// render prep
	// VAO
	glCreateVertexArrays(1, &state._VAO);

	// VBO
	state._voxel_data = std::make_unique<vox::ArrayBuffer>(
	    reinterpret_cast<const uint8_t *>(cube_vertices),
	    static_cast<int>(sizeof(cube_vertices)));

	// EBO
	state._voxel_index_data = std::make_unique<vox::ArrayBuffer>(
	    reinterpret_cast<const uint8_t *>(cube_indices),
	    static_cast<int>(sizeof(cube_indices)));

	glVertexArrayVertexBuffer(state._VAO, 0, state._voxel_data->_handle, 0, sizeof(Vertex));
	glVertexArrayElementBuffer(state._VAO, state._voxel_index_data->_handle);

	glVertexArrayAttribBinding(state._VAO, 0, 0);
	glVertexArrayAttribBinding(state._VAO, 1, 0);
	glVertexArrayAttribBinding(state._VAO, 2, 0);

	glEnableVertexArrayAttrib(state._VAO, 0);  // pos
	glEnableVertexArrayAttrib(state._VAO, 1);  // normal
	glEnableVertexArrayAttrib(state._VAO, 2);  // uv

	glVertexArrayAttribFormat(state._VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
	glVertexArrayAttribFormat(state._VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	glVertexArrayAttribFormat(state._VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));

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

	// world prep
	state._world_data = std::vector<std::vector<int>>(WORLD_SIZE_LENGTH, std::vector<int>(WORLD_SIZE_LENGTH));
	for (int x = 0; x < WORLD_SIZE_LENGTH; x++) {
		for (int z = 0; z < WORLD_SIZE_LENGTH; z++) {
			int x_shifted = x - (WORLD_SIZE_LENGTH / 2);
			int z_shifted = z - (WORLD_SIZE_LENGTH / 2);
			auto t = stb_perlin_fbm_noise3(
			    static_cast<float>(x_shifted) * 0.1f, 0.0f, static_cast<float>(z_shifted) * 0.1f,
			    2.0f,
			    0.5f,
			    5);
			int height = static_cast<int>(t * 10.0f) + 10;
			state._world_data[x][z] = height;

			state._voxel_cnt += height;
		}
	}

	// instancing VBO
	const int voxel_cnt = WORLD_SIZE_LENGTH * WORLD_SIZE_LENGTH;
	std::vector<Instance> instance_data;
	instance_data.reserve(voxel_cnt);

	for (int x = 0; x < WORLD_SIZE_LENGTH; x++) {
		for (int z = 0; z < WORLD_SIZE_LENGTH; z++) {
			int x_shifted = x - (WORLD_SIZE_LENGTH / 2);
			int z_shifted = z - (WORLD_SIZE_LENGTH / 2);
			int height = state._world_data[x][z];
			for (int y = 0; y < height; y++) {
				int texture = 0;
				if (y <= 4) {
					texture = 2;
				} else if (y <= 10) {
					texture = 1;
				} else {
					texture = 0;
				}

				instance_data.emplace_back(
				    glm::vec3{
				        static_cast<float>(x_shifted),
				        static_cast<float>(y),
				        static_cast<float>(z_shifted) },
				    texture);
			}
		}
	}

	state._instance_data = std::make_unique<vox::ArrayBuffer>(
	    reinterpret_cast<const uint8_t *>(instance_data.data()),
	    static_cast<int>(instance_data.size() * sizeof(Instance)));

	// register attribute
	glVertexArrayVertexBuffer(state._VAO, 1, state._instance_data->_handle, 0, sizeof(Instance));
	glVertexArrayBindingDivisor(state._VAO, 1, 1);

	glVertexArrayAttribBinding(state._VAO, 3, 1);
	glVertexArrayAttribFormat(state._VAO, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Instance, translation));
	glEnableVertexArrayAttrib(state._VAO, 3);

	glVertexArrayAttribBinding(state._VAO, 4, 1);
	glVertexArrayAttribIFormat(state._VAO, 4, 1, GL_INT, offsetof(Instance, texture));
	glEnableVertexArrayAttrib(state._VAO, 4);

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

	glBindVertexArray(state._VAO);
	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr, state._voxel_cnt);

	SDL_GL_SwapWindow(state._window);
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit([[maybe_unused]] void *appstate, [[maybe_unused]] SDL_AppResult result) {
	glDeleteVertexArrays(1, &state._VAO);

	SDL_GL_DestroyContext(state._context);
}
