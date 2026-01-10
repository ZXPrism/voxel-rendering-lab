#include <camera.h>

#include <glm/gtc/matrix_transform.hpp>

namespace vox {

void Camera::look_at(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up) {
	_view = glm::lookAt(eye, center, up);
}

void Camera::set_perspective(float fov_y, float aspect_ratio, float near, float far) {
	_projection = glm::perspective(fov_y, aspect_ratio, near, far);
}

glm::mat4 Camera::get_vp_matrix() const {
	return _projection * _view;
}

}  // namespace vox
