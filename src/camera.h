#pragma once

#include <glm/glm.hpp>

namespace vrl {

class Camera {
private:
	float _MoveSpeed{ 10.0f };
	float _MouseSens{ 0.1f };

	glm::vec3 _Pos{ 0.0f, 0.0f, 0.0f };
	glm::vec3 _Front, _Right;
	float _Yaw{ -90.0f }, _Pitch{ 0.0f };

	glm::mat4 _View{ 1.0f }, _Projection{ 1.0f };

public:
	Camera();

	const glm::vec3 &get_pos() const;
	const glm::mat4 &get_view() const;
	const glm::mat4 &get_projection() const;

	void update(float delta_time);

private:
	void _calc_orientation();
};

}  // namespace vrl
