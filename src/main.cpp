#include <app.h>
#include <camera.h>
#include <logger.h>
#include <shader_program.h>
#include <world/world_flat.h>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

int main() {
	using namespace vrl;

	auto &app = App::instance();
	app.init();
	app.set_flag_vsync(false);
	app.set_flag_depth_test(true);

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

	WorldFlat world_flat(1, 1, 1);

	app.run([&](float dt) {
		camera.update(dt);

		shader_program.use();
		glm::mat4 model(1.0f);
		shader_program.set_uniform("model", model);
		shader_program.set_uniform("view", camera.get_view());

		world_flat.render();
	});

	return 0;
}
