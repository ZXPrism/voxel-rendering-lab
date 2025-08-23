#include <glad/glad.h>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <app.h>
#include <logger.h>
#include <shader_program.h>
#include <vertex_buffer.h>

int main() {
	using namespace vrl;

	App app;
	app.init();

	std::vector<float> verts = {
		-0.6f, -0.5f, 0.0f,
		0.6f, -0.5f, 0.0f,
		0.0f, 0.6f, 0.0f
	};
	VertexBuffer::VertexBufferBuilder vertex_buffer_builder("vert builder", verts);
	auto vertex_buffer = vertex_buffer_builder.add_attribute(3).build();

	// prepare shaders
	ShaderProgram::ShaderProgramBuilder shader_program_builder("default_program");
	{
		Shader::ShaderBuilder vertex_shader_builder("default_vs");
		auto vertex_shader = vertex_shader_builder.set_type(ShaderType::VERTEX_SHADER).set_source_from_file("assets/test.vert").build();

		Shader::ShaderBuilder fragment_shader_builder("default_fs");
		auto fragment_shader = fragment_shader_builder.set_type(ShaderType::FRAGMENT_SHADER).set_source_from_file("assets/test.frag").build();

		shader_program_builder.add_shader(vertex_shader).add_shader(fragment_shader);
	}
	auto shader_program = shader_program_builder.build();

	app.run([&](float dt [[maybe_unused]]) {
		shader_program.use();
		vertex_buffer.use();
		glDrawArrays(GL_TRIANGLES, 0, 3);
	});

	return 0;
}
