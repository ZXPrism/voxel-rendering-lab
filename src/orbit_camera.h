#pragma once

#include <camera.h>

#include <numbers>

namespace vox {

class OrbitCamera : public Camera {
public:
	void process_input(float delta_time) override;
	void process_input_2(float delta_time, float wheel_y);  // HACK, refactor later

private:
	const float ROTATE_SPEED = 1.0f;
	const float SCROLL_SPEED = 1000.0f;

	float _radius = 60.0f;
	float _theta = static_cast<float>(std::numbers::pi / 4.0);
	float _phi = static_cast<float>(std::numbers::pi / 4.0);
};

}  // namespace vox
