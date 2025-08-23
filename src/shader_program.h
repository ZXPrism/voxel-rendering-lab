#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <interfaces/builder.h>
#include <shader_types.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace vrl {

class Shader {
private:
	std::shared_ptr<GLuint> _ShaderHandle;
	std::unordered_set<std::string> _Uniforms;

public:
	class ShaderBuilder : public IBuilder<ShaderBuilder, Shader> {
	private:
		ShaderType _ShaderType = ShaderType::UNKNOWN;
		std::string _Source;

	public:
		ShaderBuilder(const std::string &name);

		ShaderBuilder &set_type(ShaderType shader_type);
		ShaderBuilder &set_source(const std::string &shader_source);
		ShaderBuilder &set_source_from_file(const std::string &source_file_path);

		Shader _build() const;
	};

	GLuint _get_handle() const;
	const std::unordered_set<std::string> &_get_uniforms() const;

private:
	Shader() = default;
};

class ShaderProgram {
private:
	std::shared_ptr<GLuint> _Program;
	std::unordered_map<std::string, GLint> _MapUniformNameToLocation;

public:
	class ShaderProgramBuilder : public IBuilder<ShaderProgramBuilder, ShaderProgram> {
	private:
		std::vector<Shader> _Shaders;

	public:
		ShaderProgramBuilder(const std::string &name);

		ShaderProgramBuilder &add_shader(const Shader &shader);

		ShaderProgram _build() const;
	};

	void set_uniform(const std::string &name, const glm::mat4 &matrix);
	void set_uniform(const std::string &name, const glm::vec3 &vector);
	void use() const;

private:
	ShaderProgram() = default;
};

}  // namespace vrl
