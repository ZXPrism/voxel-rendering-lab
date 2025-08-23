#include <shader_program.h>

#include <logger.h>

#include <fstream>
#include <regex>
#include <sstream>

namespace vrl {

void ShaderProgram::use() const {
	glUseProgram(*_Program);
}

void ShaderProgram::set_uniform(const std::string &name, const glm::mat4 &matrix) {
	glUniformMatrix4fv(_MapUniformNameToLocation[name], 1, false, &matrix[0][0]);
}

void ShaderProgram::set_uniform(const std::string &name, const glm::vec3 &vector) {
	glUniform3fv(_MapUniformNameToLocation[name], 1, &vector[0]);
}

Shader::ShaderBuilder::ShaderBuilder(const std::string &name)
    : IBuilder(name) {
}

Shader::ShaderBuilder &Shader::ShaderBuilder::set_type(ShaderType shader_type) {
	_ShaderType = shader_type;
	return *this;
}

Shader::ShaderBuilder &Shader::ShaderBuilder::set_source(const std::string &shader_source) {
	_Source = shader_source;
	return *this;
}

Shader::ShaderBuilder &Shader::ShaderBuilder::set_source_from_file(const std::string &source_file_path) {
	std::ifstream fin(source_file_path);
	if (!fin) {
		g_logger->warn("Shader::ShaderBuilder ({}): failed to load shader source file from {}", _Name, source_file_path);
		return *this;
	}

	std::stringstream ssm;
	ssm << fin.rdbuf();
	_Source = ssm.str();
	fin.close();

	return *this;
}

Shader Shader::ShaderBuilder::_build() const {
	Shader res;

	GLuint *raw_shader_handle = new GLuint(0);
	res._ShaderHandle = std::shared_ptr<GLuint>(raw_shader_handle, [](GLuint *ptr) {
		glDeleteShader(*ptr);
		delete ptr;
	});

	switch (_ShaderType) {
	case ShaderType::VERTEX_SHADER:
		*res._ShaderHandle = glCreateShader(GL_VERTEX_SHADER);
		break;
	case ShaderType::FRAGMENT_SHADER:
		*res._ShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	case ShaderType::COMPUTE_SHADER:
		*res._ShaderHandle = glCreateShader(GL_COMPUTE_SHADER);
		break;
	default:
		g_logger->warn("Shader::ShaderBuilder ({}): unknown shader type, renderer may not work correctly", _Name);
		return res;
	}

	const char *const shader_src = _Source.c_str();
	glShaderSource(*res._ShaderHandle, 1, &shader_src, nullptr);
	glCompileShader(*res._ShaderHandle);

	GLint compile_status;
	glGetShaderiv(*res._ShaderHandle, GL_COMPILE_STATUS, &compile_status);
	if (!compile_status) {
		static char compile_log[1024];
		glGetShaderInfoLog(*res._ShaderHandle, sizeof(compile_log), nullptr, compile_log);
		g_logger->warn("Shader::ShaderBuilder ({}): shader compilation failed:\n{}", _Name, compile_log);
	}

	std::regex pattern(R"(uniform\s+(\w+)\s+(\w+);)");
	std::smatch match;
	auto targetIter = _Source.cbegin();
	while (std::regex_search(targetIter, _Source.cend(), match, pattern)) {
		targetIter = match[0].second;
		res._Uniforms.insert(match[2].str());
		g_logger->info("Shader::ShaderBuilder ({}): detected uniform {} with type {}", _Name, match[1].str(), match[2].str());
	}

	return res;
}

GLuint Shader::_get_handle() const {
	return *_ShaderHandle;
}

const std::unordered_set<std::string> &Shader::_get_uniforms() const {
	return _Uniforms;
}

ShaderProgram::ShaderProgramBuilder::ShaderProgramBuilder(const std::string &name)
    : IBuilder(name) {
}

ShaderProgram::ShaderProgramBuilder &ShaderProgram::ShaderProgramBuilder::add_shader(const Shader &shader) {
	_Shaders.push_back(shader);
	return *this;
}

ShaderProgram ShaderProgram::ShaderProgramBuilder::_build() const {
	ShaderProgram res;

	GLuint *raw_program_handle = new GLuint(glCreateProgram());
	res._Program = std::shared_ptr<GLuint>(raw_program_handle, [](GLuint *ptr) {
		glDeleteProgram(*ptr);
		delete ptr;
	});

	for (const auto &shader : _Shaders) {
		glAttachShader(*res._Program, shader._get_handle());
	}
	glLinkProgram(*res._Program);

	GLint link_status;
	glGetProgramiv(*res._Program, GL_LINK_STATUS, &link_status);
	if (!link_status) {
		static char link_log[1024];
		glGetProgramInfoLog(*res._Program, sizeof(link_log), nullptr, link_log);
		g_logger->warn("ShaderProgram::ShaderProgramBuilder ({}): program link failed:\n{}", _Name, link_log);
	}

	// collect uniforms and query their locations in advance
	for (const auto &shader : _Shaders) {
		auto &uniform_names = shader._get_uniforms();
		for (const auto &uniform_name : uniform_names) {
			if (!res._MapUniformNameToLocation.contains(uniform_name)) {
				res._MapUniformNameToLocation[uniform_name] = glGetUniformLocation(*res._Program, uniform_name.c_str());
				g_logger->info("ShaderProgram::ShaderProgramBuilder ({}): uniform {}'s location is {}",
				               _Name,
				               uniform_name,
				               res._MapUniformNameToLocation[uniform_name]);
			}
		}
	}

	return res;
}

}  // namespace vrl
