#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include <interfaces/builder.h>

#include <glad/glad.h>

namespace vrl {

enum class ShaderType {
	UNKNOWN,
	VERTEX_SHADER,
	FRAGMENT_SHADER,
	COMPUTE_SHADER,
};

enum class ShaderDataType {
	INT,
	FLOAT,
	VEC2,
	VEC3,
	VEC4,
	MAT2,
	MAT3,
	MAT4,
};

class Shader {
private:
	std::shared_ptr<GLuint> _ShaderHandle;
	std::unordered_set<std::string> _Uniforms;

public:
	class ShaderBuilder : IBuilder<ShaderBuilder> {
	private:
		ShaderType _ShaderType = ShaderType::UNKNOWN;
		std::string _Source;

	public:
		ShaderBuilder(const std::string &name);

		ShaderBuilder &set_type(ShaderType shader_type);
		ShaderBuilder &set_source(const std::string &shader_source);
		ShaderBuilder &set_source_from_file(const std::string &source_file_path);

		Shader build() const;
	};

	GLuint get_handle() const;

private:
	Shader() = default;
};

class ShaderProgram {
private:
	std::shared_ptr<GLuint> _Program;

public:
	class ShaderProgramBuilder : IBuilder<ShaderProgramBuilder> {
	private:
		std::vector<Shader> _Shaders;

	public:
		ShaderProgramBuilder(const std::string &name);

		ShaderProgramBuilder &add_shader(const Shader &shader);

		ShaderProgram build() const;
	};

	void use();

private:
	ShaderProgram() = default;
};

}  // namespace vrl
