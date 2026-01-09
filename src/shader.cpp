#include <shader.h>

#include <logger.h>
#include <utils.h>

namespace vox {

Shader::Shader(const std::string &vs_path, const std::string &fs_path) {
	auto load_shader = [](GLenum shader_type, const std::string &shader_path) -> std::optional<GLuint> {
		auto shader = glCreateShader(shader_type);
		const std::string shader_src = utils::load_whole_file(shader_path);
		const char *const shader_src_ptr = shader_src.c_str();
		glShaderSource(shader, 1, &shader_src_ptr, nullptr);
		glCompileShader(shader);

		GLint ok = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
		if (ok == 0) {
			GLint len = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
			std::string log(len, '\0');
			glGetShaderInfoLog(shader, len, nullptr, log.data());
			SPDLOG_WARN("failed to compile shader from \"{}\" of type {}: {}", shader_path, shader_type, log);

			glDeleteShader(shader);
			return std::nullopt;
		}

		return shader;
	};

	auto vs = load_shader(GL_VERTEX_SHADER, vs_path);
	auto fs = load_shader(GL_FRAGMENT_SHADER, fs_path);

	if (!vs.has_value() || !fs.has_value()) {
		SPDLOG_WARN("since some shaders can not compile, shader program won't be created");
	} else {
		// compile shader program
		auto shader_program = glCreateProgram();

		glAttachShader(shader_program, *vs);
		glAttachShader(shader_program, *fs);

		glLinkProgram(shader_program);

		GLint ok = 0;
		glGetProgramiv(shader_program, GL_LINK_STATUS, &ok);
		if (ok == 0) {
			GLint len = 0;
			glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &len);
			std::string log(len, '\0');
			glGetProgramInfoLog(shader_program, len, nullptr, log.data());
			SPDLOG_WARN("failed to compile shader program: {}", log);
		}

		glDetachShader(shader_program, *vs);
		glDeleteShader(*vs);

		glDetachShader(shader_program, *fs);
		glDeleteShader(*fs);

		_program = shader_program;
	}
}

void Shader::use_program() const {
	glUseProgram(_program);
}

void Shader::set_uniform(const std::string &name, const glm::mat4 &matrix) {
	auto location = _get_uniform_location(name);
	if (location != -1) {
		glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
	}
}

void Shader::set_uniform(const std::string &name, int value) {
	auto location = _get_uniform_location(name);
	if (location != -1) {
		glUniform1i(location, value);
	}
}

void Shader::set_uniform(const std::string &name, const glm::vec3 &vector) {
	auto location = _get_uniform_location(name);
	if (location != -1) {
		glUniform3fv(location, 1, &vector[0]);
	}
}

GLint Shader::_get_uniform_location(const std::string &name) {
	GLint location = -1;

	const auto iter = _map_uniform_name_to_location.find(name);
	if (iter != _map_uniform_name_to_location.end()) {
		location = iter->second;
	} else {
		location = glGetUniformLocation(_program, name.c_str());
		if (location == -1) {
			SPDLOG_WARN("uniform <{}> does not exist", name);
			return -1;
		}
		_map_uniform_name_to_location[name] = location;
	}

	return location;
}

}  // namespace vox
