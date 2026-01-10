#pragma once

#include <glm/glm.hpp>

namespace vox {

class Camera {
public:
	virtual ~Camera() = default;

	void look_at(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up = { 0.0f, 1.0f, 0.0f });
	void set_perspective(float fov_y, float aspect_ratio, float near, float far);
	[[nodiscard]] glm::mat4 get_vp_matrix() const;

protected:
	glm::mat4 _view;
	glm::mat4 _projection;
};

}  // namespace vox
