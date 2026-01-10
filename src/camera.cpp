#include <camera.h>

#include <glm/gtc/matrix_transform.hpp>

namespace vox {

void Camera::look_at(const glm::vec3 &center, const glm::vec3 &up) {
	_view = glm::lookAt(_pos, center, up);
}

void Camera::set_perspective(float fov_y, float aspect_ratio, float near, float far) {
	_projection = glm::perspective(fov_y, aspect_ratio, near, far);
}

glm::vec3 Camera::get_pos() const {
	return _pos;
}

glm::mat4 Camera::get_vp_matrix() const {
	return _projection * _view;
}

glm::mat4 Camera::get_view_matrix() const {
	return _view;
}

glm::mat4 Camera::get_projection_matrix() const {
	return _projection;
}

}  // namespace vox
