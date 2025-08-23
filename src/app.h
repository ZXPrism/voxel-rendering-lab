#pragma once

#include <config.h>
#include <interfaces/singleton.h>

#include <functional>

struct GLFWwindow;

namespace vrl {

using OnCursorPosFunc = std::function<void(double x_pos, double y_pos)>;
using OnKeyFunc = std::function<void(int key, int scan_code, int action, int mods)>;
using OnMouseButtonFunc = std::function<void(int button, int action, int mods)>;
using OnScrollFunc = std::function<void(double x_offset, double y_offset)>;
using OnWindowSizeFunc = std::function<void(int width, int height)>;

class App : public Singleton<App> {
private:
	GLFWwindow *_Window;

	int _WindowWidth = config::initial_window_width;
	int _WindowHeight = config::initial_window_height;

	std::vector<OnCursorPosFunc> _OnCursorPosVec;
	std::vector<OnKeyFunc> _OnKeyVec;
	std::vector<OnMouseButtonFunc> _OnMouseButtonVec;
	std::vector<OnScrollFunc> _OnScrollVec;
	std::vector<OnWindowSizeFunc> _OnWindowSizeVec;

public:
	void init();
	void run(const std::function<void(float delta_time)> &callback);
	void shutdown();

	void set_flag_vsync(bool flag) const;
	void set_flag_depth_test(bool flag) const;

	float get_aspect_ratio() const;
	bool check_key_pressed(int key) const;

	void register_on_key_func(OnKeyFunc func);
	void register_on_mouse_button_func(OnMouseButtonFunc func);
	void register_on_cursor_pos_func(OnCursorPosFunc func);
	void register_on_scroll_func(OnScrollFunc func);
	void register_on_window_size_func(OnWindowSizeFunc func);

	void on_key(int key, int scan_code, int action, int mods);
	void on_mouse_button(int button, int action, int mods);
	void on_cursor_pos(double x_pos, double y_pos);
	void on_scroll(double x_offset, double y_offset);
	void on_window_size(int width, int height);

private:
	void _init_window();
	void _init_opengl();
	void _init_IMGUI();
	void _init_IMGUI_styles();
	void _init_IMGUI_fonts();
	void _init_callbacks();
	void _shutdown_window();
	void _shutdown_IMGUI();
};

}  // namespace vrl
