#include <app.h>

#include <chrono>
#include <format>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <logger.h>

namespace vrl {

void App::init() {
	g_logger->info("App: initializing app...");

	_init_window();
	_init_opengl();
	_init_IMGUI();
	_init_IMGUI_styles();
	_init_IMGUI_fonts();
	_init_callbacks();
}

void App::run(const std::function<void(float)> &callback) {
	g_logger->info("App: running main loop...");

	auto t1 = std::chrono::steady_clock::now();
	float delta_time = 0.0f;
	float delta_time_cnt = 0.0f;
	int fps_smooth = 0;

	while (!glfwWindowShouldClose(_Window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		callback(delta_time);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("control");
		{
			static bool wireframe = false;
			if (ImGui::Checkbox("wireframe mode", &wireframe)) {
				if (wireframe) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				} else {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwPollEvents();
		glfwSwapBuffers(_Window);

		auto t2 = std::chrono::steady_clock::now();
		delta_time = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() / 1e6);
		delta_time_cnt += delta_time;
		t1 = t2;

		if (delta_time_cnt >= 0.5f) {
			delta_time_cnt = 0.0f;

			fps_smooth = static_cast<int>(fps_smooth * config::fps_lerp_coeff + 1.0f / delta_time * (1 - config::fps_lerp_coeff));
			glfwSetWindowTitle(_Window, std::format("{} - fps: {}", config::window_title, fps_smooth).c_str());
		}
	}
}

void App::shutdown() {
	g_logger->info("App: shutting down voxelz...");

	_shutdown_IMGUI();
	_shutdown_window();
}

float App::get_aspect_ratio() const {
	return 1.0f * _WindowWidth / _WindowHeight;
}

void App::_init_callbacks() {
	glfwSetWindowUserPointer(_Window, this);

	// since IMGUI has already installed callbacks
	// we need to hook into them
	auto prev_cursor_pos_callback =
	    glfwSetCursorPosCallback(_Window, [](GLFWwindow *window, double x_pos, double y_pos) {
		    static_cast<App *>(glfwGetWindowUserPointer(window))->on_cursor_pos(x_pos, y_pos);
	    });
	if (prev_cursor_pos_callback) {
		register_on_cursor_pos_func(std::bind_front(prev_cursor_pos_callback, _Window));
	}

	auto prev_key_callback =
	    glfwSetKeyCallback(_Window, [](GLFWwindow *window, int key, int scan_code, int action, int mods) {
		    static_cast<App *>(glfwGetWindowUserPointer(window))->on_key(key, scan_code, action, mods);
	    });
	if (prev_key_callback) {
		register_on_key_func(std::bind_front(prev_key_callback, _Window));
	}

	auto prev_mouse_button_callback =
	    glfwSetMouseButtonCallback(_Window, [](GLFWwindow *window, int button, int action, int mods) {
		    static_cast<App *>(glfwGetWindowUserPointer(window))->on_mouse_button(button, action, mods);
	    });
	if (prev_mouse_button_callback) {
		register_on_mouse_button_func(std::bind_front(prev_mouse_button_callback, _Window));
	}

	auto prev_scroll_callback =
	    glfwSetScrollCallback(_Window, [](GLFWwindow *window, double x_offset, double y_offset) {
		    static_cast<App *>(glfwGetWindowUserPointer(window))->on_scroll(x_offset, y_offset);
	    });
	if (prev_scroll_callback) {
		register_on_scroll_func(std::bind_front(prev_scroll_callback, _Window));
	}

	auto prev_window_size_callback =
	    glfwSetWindowSizeCallback(_Window, [](GLFWwindow *window, int width, int height) {
		    static_cast<App *>(glfwGetWindowUserPointer(window))->on_window_size(width, height);
	    });
	if (prev_window_size_callback) {
		register_on_window_size_func(std::bind_front(prev_window_size_callback, _Window));
	}

	register_on_window_size_func([&](int width, int height) {
		_WindowWidth = width;
		_WindowHeight = height;

		glViewport(0, 0, width, height);
	});

	register_on_key_func([&](int key, int scan_code [[maybe_unused]], int action, int mods [[maybe_unused]]) {
		static bool cursor_enable = false;

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			cursor_enable = !cursor_enable;
			if (cursor_enable) {
				glfwSetInputMode(_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			} else {
				glfwSetInputMode(_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}
	});
}

void App::_init_window() {
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config::opengl_ver_major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config::opengl_ver_minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	_Window = glfwCreateWindow(_WindowWidth, _WindowHeight, config::window_title, nullptr, nullptr);
	if (!_Window) {
		g_logger->error("App: failed to create window");
		return;
	}

	glfwMakeContextCurrent(_Window);

	// center the window
	auto vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(_Window, (vidmode->width - _WindowWidth) / 2, (vidmode->height - _WindowHeight) / 2);

	// vsync
	glfwSwapInterval(0);  // 0: off, 1: on

	glfwSetInputMode(_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void App::_init_opengl() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		g_logger->error("App: fail to init opengl");
		return;
	}

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glViewport(0, 0, _WindowWidth, _WindowHeight);
}

void App::_init_IMGUI() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(_Window, true);
	ImGui_ImplOpenGL3_Init(config::glsl_ver);
	ImGui::StyleColorsDark();
}

void App::_init_IMGUI_styles() {

	auto &style = ImGui::GetStyle();
	auto &colors = style.Colors;

	colors[ImGuiCol_TitleBg] = { 0.113f, 0.113f, 0.113f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = { 0.187f, 0.187f, 0.187f, 1.0f };

	colors[ImGuiCol_Tab] = { 0.113f, 0.113f, 0.113f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = { 0.113f, 0.113f, 0.113f, 1.0f };
	colors[ImGuiCol_TabActive] = { 0.5f, 0.5f, 0.5f, 1.0f };
	colors[ImGuiCol_TabHovered] = { 0.3f, 0.3f, 0.3f, 1.0f };

	colors[ImGuiCol_Header] = { 0.4f, 0.4f, 0.4f, 1.0f };
	colors[ImGuiCol_HeaderHovered] = { 0.6f, 0.6f, 0.6f, 1.0f };

	colors[ImGuiCol_FrameBg] = { 0.3f, 0.3f, 0.3f, 1.0f };

	colors[ImGuiCol_Button] = { 0.5f, 0.5f, 0.8f, 1.0f };

	style.WindowPadding = { 8, 10 };
	style.FramePadding = { 8, 8 };
	style.ItemSpacing = { 8, 8 };
	style.ItemInnerSpacing = { 8, 8 };
	style.ScrollbarSize = 16;
	style.FrameRounding = 4;
	style.WindowRounding = 8;
	style.ChildRounding = 8;
}

void App::_init_IMGUI_fonts() {
	auto &io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("assets/fonts/NotoSansCJK-Regular.otf", 20.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
}

void App::_shutdown_window() {
	glfwDestroyWindow(_Window);
	glfwTerminate();
}

void App::_shutdown_IMGUI() {
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
}

void App::register_on_key_func(OnKeyFunc func) {
	_OnKeyVec.push_back(func);
}

void App::register_on_mouse_button_func(OnMouseButtonFunc func) {
	_OnMouseButtonVec.push_back(func);
}

void App::register_on_cursor_pos_func(OnCursorPosFunc func) {
	_OnCursorPosVec.push_back(func);
}

void App::register_on_scroll_func(OnScrollFunc func) {
	_OnScrollVec.push_back(func);
}

void App::register_on_window_size_func(OnWindowSizeFunc func) {
	_OnWindowSizeVec.push_back(func);
}

void App::on_cursor_pos(double xPos, double yPos) {
	for (auto &fn : _OnCursorPosVec) {
		fn(xPos, yPos);
	}
}

void App::on_key(int key, int scanCode, int action, int mods) {
	for (auto &fn : _OnKeyVec) {
		fn(key, scanCode, action, mods);
	}
}

void App::on_mouse_button(int button, int action, int mods) {
	for (auto &fn : _OnMouseButtonVec) {
		fn(button, action, mods);
	}
}

void App::on_scroll(double xOffset, double yOffset) {
	for (auto &fn : _OnScrollVec) {
		fn(xOffset, yOffset);
	}
}

void App::on_window_size(int width, int height) {
	for (auto &fn : _OnWindowSizeVec) {
		fn(width, height);
	}
}

bool App::check_key_pressed(int key) const {
	return glfwGetKey(_Window, key);
}

}  // namespace vrl
