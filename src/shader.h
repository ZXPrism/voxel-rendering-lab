#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace vox {

class Shader {
public:
	explicit Shader(const std::string &vs_path, const std::string &fs_path);

	void use_program() const;
	void set_uniform(const std::string &name, const glm::vec3 &vector);
	void set_uniform(const std::string &name, const glm::mat4 &matrix);
	void set_uniform(const std::string &name, int value);

private:
	GLuint _program;
	std::unordered_map<std::string, GLint> _map_uniform_name_to_location;  // cache

	GLint _get_uniform_location(const std::string &name);
};

}  // namespace vox
