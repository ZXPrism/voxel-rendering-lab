#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array_buffer.h>
#include <cube_texture.h>
#include <logger.h>
#include <orbit_camera.h>
#include <render_pass.h>
#include <shader.h>
#include <texture.h>
#include <vertex_array.h>
#include <vertices.h>
#include <world.h>

#include <chrono>
#include <memory>

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;

struct {
	float _delta_time = 0.0f;

	SDL_Window *_window;
	SDL_GLContextState *_context;

	std::unique_ptr<vox::Shader> _shader;
	std::unique_ptr<vox::Shader> _skybox_shader;

	vox::OrbitCamera _camera;

	std::unique_ptr<vox::CubeTexture> _skybox;
	std::unique_ptr<vox::Texture> _grass_block;
	std::unique_ptr<vox::Texture> _dirt_block;
	std::unique_ptr<vox::Texture> _stone_block;

	std::unique_ptr<vox::VertexArray> _skybox_vertex_array;
	std::unique_ptr<vox::ArrayBuffer> _skybox_data;

	std::optional<vox::RenderPass> _geometry_pass;
	std::optional<vox::RenderPass> _color_pass;
	std::optional<vox::RenderPass> _default_pass;

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
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		SDL_GL_SetSwapInterval(0);  // no vsync

		return true;
	}

	void prep() {
		// shaders
		_shader = std::make_unique<vox::Shader>("assets/shader/cube.vert", "assets/shader/cube.frag");
		_skybox_shader = std::make_unique<vox::Shader>("assets/shader/skybox.vert", "assets/shader/skybox.frag");

		// textures
		_skybox = std::make_unique<vox::CubeTexture>("assets/texture/skybox");
		_grass_block = std::make_unique<vox::Texture>("assets/texture/grass_block.png");
		_dirt_block = std::make_unique<vox::Texture>("assets/texture/dirt_block.png");
		_stone_block = std::make_unique<vox::Texture>("assets/texture/stone_block.png");

		// skybox
		_skybox_vertex_array = std::make_unique<vox::VertexArray>();
		_skybox_data = std::make_unique<vox::ArrayBuffer>(
		    reinterpret_cast<const uint8_t *>(vox::skybox_vertices),
		    static_cast<int>(sizeof(vox::skybox_vertices)),
		    0);

		glVertexArrayVertexBuffer(_skybox_vertex_array->_handle, 0, _skybox_data->_handle, 0, sizeof(vox::SkyboxVertex));
		glVertexArrayAttribBinding(_skybox_vertex_array->_handle, 0, 0);
		glVertexArrayAttribFormat(_skybox_vertex_array->_handle, 0, 3, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(_skybox_vertex_array->_handle, 0);

		// camera
		_camera.set_perspective(
		    glm::radians(90.0f),
		    static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
		    0.1f, 1000.0f);

		// world
		_world.generate();

		// deferred
		// _geometry_pass = vox::RenderPass::RenderPassBuilder("geometry_pass")
		//                      .add_color_attachment(color_r, true, { 1.0, 0.0, 0.0, 1.0 })
		//                      .add_color_attachment(color_g, true, { 0.0, 1.0, 0.0, 1.0 })
		//                      .add_color_attachment(color_b, true, { 0.0, 0.0, 1.0, 1.0 })
		//                      .build();
		// _color_pass = vox::RenderPass::RenderPassBuilder("color_pass")
		//                   .add_color_attachment(color_r, true, { 1.0, 0.0, 0.0, 1.0 })
		//                   .add_color_attachment(color_g, true, { 0.0, 1.0, 0.0, 1.0 })
		//                   .add_color_attachment(color_b, true, { 0.0, 0.0, 1.0, 1.0 })
		//                   .build();
		_default_pass = vox::RenderPass::RenderPassBuilder("default_pass").build();
	}

	void update() {

		// logic
		_camera.process_input(_delta_time);

		// render
		auto default_pass_config = vox::RenderPassConfig{ ._enable_depth_test = true, ._enable_srgb = false };
		_default_pass->use(default_pass_config, [&]() {
			auto &shader = _shader;
			shader->use_program();
			shader->set_uniform("uProjectionView", _camera.get_vp_matrix());
			shader->set_uniform("uCameraPos", _camera.get_pos());
			shader->set_uniform("uBlockTextureGrass", 0);
			shader->set_uniform("uBlockTextureDirt", 1);
			shader->set_uniform("uBlockTextureStone", 2);

			_grass_block->bind_texture(0);
			_dirt_block->bind_texture(1);
			_stone_block->bind_texture(2);

			_world.render();

			_skybox_shader->use_program();
			_skybox_shader->set_uniform("uSkyboxTexture", 0);
			_skybox_shader->set_uniform("uProjection", _camera.get_projection_matrix());
			_skybox_shader->set_uniform("uView", glm::mat4(glm::mat3(_camera.get_view_matrix())));
			_skybox->bind_texture(0);
			glDepthFunc(GL_LEQUAL);
			_skybox_vertex_array->use();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDepthFunc(GL_LESS);
		});

		SDL_GL_SwapWindow(_window);
	}

	void shutdown() const {
		SDL_GL_DestroyContext(_context);
	}
} global_state;

SDL_AppResult SDL_AppInit([[maybe_unused]] void **appstate, [[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
	SDL_SetAppMetadata("vox", "1.0", "com.zxp4.voxelrenderinglab");

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
		global_state._camera.process_input_2(event->wheel.y);
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
