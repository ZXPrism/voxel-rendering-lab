#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <app.h>
#include <logger.h>
#include <shader_program.h>

int main() {
	vrl::g_logger->info("renderer on..");

	vrl::App app;
	app.init();

	// prepare vertex data
	float verts[] = {
		-0.6f, -0.5f, 0.0f,
		0.6f, -0.5f, 0.0f,
		0.0f, 0.6f, 0.0f
	};
	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// prepare shaders
	vrl::Shader::ShaderBuilder vertex_shader_builder("default_vs");
	auto vertex_shader = vertex_shader_builder.set_type(vrl::ShaderType::VERTEX_SHADER).set_source_from_file("assets/test.vert").build();

	vrl::Shader::ShaderBuilder fragment_shader_builder("default_fs");
	auto fragment_shader = fragment_shader_builder.set_type(vrl::ShaderType::FRAGMENT_SHADER).set_source_from_file("assets/test.frag").build();

	vrl::ShaderProgram::ShaderProgramBuilder shader_program_builder("default_program");
	shader_program_builder.add_shader(vertex_shader).add_shader(fragment_shader);
	auto shader_program = shader_program_builder.build();

	app.run([&](float dt [[maybe_unused]]) {
		shader_program.use();

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	});

	// free resources
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	return 0;
}
