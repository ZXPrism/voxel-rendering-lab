#pragma once

#include <camera.h>

namespace vox {

class OrbitCamera : public Camera {
public:
	void process_input(float delta_time) override;
	void process_input_2(float wheel_y);  // HACK, refactor later

	glm::vec3 get_view_ray() override;

private:
	const float ROTATE_SPEED = 1.0f;
	const float SCROLL_SPEED = 10.0f;

	float _radius = 85.0f;
	float _theta = 0.0f;
	float _phi = 0.0f;
};

}  // namespace vox
