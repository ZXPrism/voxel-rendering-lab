#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <logger.h>
#include <shader.h>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

#include <memory>

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 960;

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct {
	SDL_Window *_window;
	SDL_GLContextState *_context;

	std::unique_ptr<vox::Shader> _shader;
	GLuint _VAO;
	GLuint _VBO;
	GLuint _EBO;
	glm::mat4 _pv;
} state;

static const Vertex cube_vertices[] = {
	// +X
	{ .pos = { 0.5f, -0.5f, -0.5f }, .normal = { 1, 0, 0 }, .uv = { 0, 0 } },
	{ .pos = { 0.5f, 0.5f, -0.5f }, .normal = { 1, 0, 0 }, .uv = { 1, 0 } },
	{ .pos = { 0.5f, 0.5f, 0.5f }, .normal = { 1, 0, 0 }, .uv = { 1, 1 } },
	{ .pos = { 0.5f, -0.5f, 0.5f }, .normal = { 1, 0, 0 }, .uv = { 0, 1 } },
	// -X
	{ .pos = { -0.5f, -0.5f, 0.5f }, .normal = { -1, 0, 0 }, .uv = { 0, 0 } },
	{ .pos = { -0.5f, 0.5f, 0.5f }, .normal = { -1, 0, 0 }, .uv = { 1, 0 } },
	{ .pos = { -0.5f, 0.5f, -0.5f }, .normal = { -1, 0, 0 }, .uv = { 1, 1 } },
	{ .pos = { -0.5f, -0.5f, -0.5f }, .normal = { -1, 0, 0 }, .uv = { 0, 1 } },
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
	{ .pos = { -0.5f, -0.5f, 0.5f }, .normal = { 0, 0, 1 }, .uv = { 0, 0 } },
	{ .pos = { 0.5f, -0.5f, 0.5f }, .normal = { 0, 0, 1 }, .uv = { 1, 0 } },
	{ .pos = { 0.5f, 0.5f, 0.5f }, .normal = { 0, 0, 1 }, .uv = { 1, 1 } },
	{ .pos = { -0.5f, 0.5f, 0.5f }, .normal = { 0, 0, 1 }, .uv = { 0, 1 } },
	// -Z
	{ .pos = { 0.5f, -0.5f, -0.5f }, .normal = { 0, 0, -1 }, .uv = { 0, 0 } },
	{ .pos = { -0.5f, -0.5f, -0.5f }, .normal = { 0, 0, -1 }, .uv = { 1, 0 } },
	{ .pos = { -0.5f, 0.5f, -0.5f }, .normal = { 0, 0, -1 }, .uv = { 1, 1 } },
	{ .pos = { 0.5f, 0.5f, -0.5f }, .normal = { 0, 0, -1 }, .uv = { 0, 1 } },
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

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	SDL_GL_SetSwapInterval(0);  // no vsync

	SPDLOG_INFO("OpenGL Version: {}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));
	SPDLOG_INFO("GLSL Version: {}", reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
	SPDLOG_INFO("Renderer:{}", reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
	SPDLOG_INFO("Vendor: {}", reinterpret_cast<const char *>(glGetString(GL_VENDOR)));

	// render prep
	// VAO
	glCreateVertexArrays(1, &state._VAO);

	// VBO
	glCreateBuffers(1, &state._VBO);
	glNamedBufferStorage(
	    state._VBO,
	    sizeof(cube_vertices),
	    cube_vertices,
	    0);

	// EBO
	glCreateBuffers(1, &state._EBO);
	glNamedBufferStorage(
	    state._EBO,
	    sizeof(cube_indices),
	    cube_indices,
	    0);

	glVertexArrayVertexBuffer(state._VAO, 0, state._VBO, 0, sizeof(Vertex));
	glVertexArrayElementBuffer(state._VAO, state._EBO);

	glEnableVertexArrayAttrib(state._VAO, 0);  // pos
	glEnableVertexArrayAttrib(state._VAO, 1);  // normal
	glEnableVertexArrayAttrib(state._VAO, 2);  // uv

	glVertexArrayAttribFormat(state._VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
	glVertexArrayAttribFormat(state._VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	glVertexArrayAttribFormat(state._VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));

	glVertexArrayAttribBinding(state._VAO, 0, 0);
	glVertexArrayAttribBinding(state._VAO, 1, 0);
	glVertexArrayAttribBinding(state._VAO, 2, 0);

	state._shader = std::make_unique<vox::Shader>("shader/cube.vert", "shader/cube.frag");

	glm::mat4 view = glm::lookAt(
	    glm::vec3(80, 80, 80),
	    glm::vec3(0, 0, 0),
	    glm::vec3(0, 1, 0));
	glm::mat4 proj = glm::perspective(
	    glm::radians(90.0f),
	    static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
	    0.1f, 1000.0f);
	state._pv = proj * view;

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

	glBindVertexArray(state._VAO);

	[[maybe_unused]] int total = 0;  // 163876
	for (int x = -64; x < 64; x++) {
		for (int z = -64; z < 64; z++) {
			auto t = stb_perlin_fbm_noise3(
			    static_cast<float>(x) * 0.1f, 0.0f, static_cast<float>(z) * 0.1f,
			    2.0f,
			    0.5f,
			    5);
			int height = static_cast<int>(t * 10.0f) + 10;
			total += height;

			for (int y = 0; y < height; y++) {

				glm::mat4 model{ 1.0f };
				model = glm::translate(model, glm::vec3{ x, y, z });
				shader->set_uniform("uModel", model);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
			}
		}
	}

	SDL_GL_SwapWindow(state._window);
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit([[maybe_unused]] void *appstate, [[maybe_unused]] SDL_AppResult result) {
	SDL_GL_DestroyContext(state._context);
}
