#pragma once

namespace vrl {

namespace config {

constexpr int initial_window_width [[maybe_unused]] = 1280;
constexpr int initial_window_height [[maybe_unused]] = 720;
constexpr const char *window_title [[maybe_unused]] = "Voxel Rendering Lab";
constexpr int opengl_ver_major [[maybe_unused]] = 4;
constexpr int opengl_ver_minor [[maybe_unused]] = 6;
constexpr const char *glsl_ver [[maybe_unused]] = "#version 460";  // for imgui init only
constexpr float fps_lerp_coeff [[maybe_unused]] = 0.5f;

}  // namespace config

}  // namespace vrl
