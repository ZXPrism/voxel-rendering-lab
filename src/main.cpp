#include <app.h>
#include <camera.h>
#include <logger.h>
#include <render_pass.h>
#include <shader_program.h>
#include <world/world_flat.h>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include <random>

int main() {
	using namespace vrl;

	auto &app = App::instance();
	app.init();
	app.set_flag_vsync(false);
	app.set_flag_depth_test(true);
	app.set_clear_color({ 0.0f, 0.0f, 0.0f });

	// prepare camera
	Camera camera;

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
	shader_program.use();
	shader_program.set_uniform("projection", camera.get_projection());

	// SSAO shaders
	// Common fullscreen quad vertex shader (reused by multiple programs)
	Shader::ShaderBuilder screen_quad_vs_builder("screen_quad_vs");
	auto screen_quad_vs = screen_quad_vs_builder
	                          .set_type(ShaderType::VERTEX_SHADER)
	                          .set_source_from_file("assets/screen_quad.vert")
	                          .build();

	// ssao quad
	// Fullscreen quad vertices (position and texture coordinates)
	std::vector<float> quadVertices = {
		// Positions   // TexCoords
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
	// Create the fullscreen quad vertex buffer
	auto quadVertexBuffer = VertexBuffer::VertexBufferBuilder("fullscreen_quad", quadVertices)
	                            .add_attribute(2)  // position (vec2)
	                            .add_attribute(2)  // texture coordinates (vec2)
	                            .build();

	WorldFlat world_flat(50, 50, 10);
	world_flat.init();

	app.run([&](float dt) {
		camera.update(dt);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// // baseline
		if (0) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			shader_program.use();
			shader_program.set_uniform("view", camera.get_view());
			world_flat.render(shader_program);
		} else {
			// 1. Geometry Pass
			geometry_pass.use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			geometryShader.use();
			geometryShader.set_uniform("projection", camera.get_projection());
			geometryShader.set_uniform("view", camera.get_view());
			world_flat.render(geometryShader);

			quadVertexBuffer.use();
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	});

	app.shutdown();

	return 0;
}
