#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <logger.h>
#include <shader_program.h>

static void framebuffer_size_callback(GLFWwindow *, int w, int h) {
	glViewport(0, 0, w, h);
}

int main() {
	vrl::g_logger->info("renderer on..");

	// init glfw
	if (!glfwInit()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);  // MSAA request
	GLFWwindow *win = glfwCreateWindow(800, 600, "Mono Triangle", nullptr, nullptr);
	if (!win) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(win);
	glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);

	// init opengl
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "GLAD load failed\n");
		return -1;
	}
	glEnable(GL_MULTISAMPLE);

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
	vrl::Shader::ShaderBuilder vertex_shader_builder("default");
	auto vertex_shader = vertex_shader_builder.set_type(vrl::ShaderType::VERTEX_SHADER).set_source_from_file("assets/test.vert").build();

	vrl::Shader::ShaderBuilder fragment_shader_builder("default");
	auto fragment_shader = fragment_shader_builder.set_type(vrl::ShaderType::FRAGMENT_SHADER).set_source_from_file("assets/test.frag").build();

	vrl::ShaderProgram::ShaderProgramBuilder shader_program_builder;
	shader_program_builder.add_shader(vertex_shader).add_shader(fragment_shader);
	auto shader_program = shader_program_builder.build();

	glClearColor(0.08f, 0.09f, 0.10f, 1.0f);
	while (!glfwWindowShouldClose(win)) {
		glClear(GL_COLOR_BUFFER_BIT);

		shader_program.use();

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	// free resources
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	glfwDestroyWindow(win);
	glfwTerminate();

	return 0;
}
