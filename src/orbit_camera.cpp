#include <orbit_camera.h>

#include <SDL3/SDL.h>

namespace vox {

void OrbitCamera::process_input(float delta_time) {
	const auto *keystate = SDL_GetKeyboardState(nullptr);

	if (keystate[SDL_SCANCODE_A]) {
		_phi -= ROTATE_SPEED * delta_time;
	}
	if (keystate[SDL_SCANCODE_D]) {
		_phi += ROTATE_SPEED * delta_time;
	}
	if (keystate[SDL_SCANCODE_W]) {
		_theta -= ROTATE_SPEED * delta_time;
	}
	if (keystate[SDL_SCANCODE_S]) {
		_theta += ROTATE_SPEED * delta_time;
	}

	_theta = glm::clamp(_theta, 0.1f, static_cast<float>(std::numbers::pi) - 0.1f);

	glm::vec3 eye{
		_radius * glm::sin(_theta) * glm::sin(_phi),
		_radius * glm::cos(_theta),
		_radius * glm::sin(_theta) * glm::cos(_phi)
	};
	look_at(eye, glm::vec3{ 0.0f });
}

void OrbitCamera::process_input_2(float delta_time, float wheel_y) {
	_radius -= SCROLL_SPEED * delta_time * wheel_y;
	_radius = std::max(_radius, 0.1f);
}

}  // namespace vox
