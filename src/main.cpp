#include <app.h>
#include <camera.h>
#include <logger.h>
#include <render_pass.h>
#include <shader_program.h>
#include <vertices.h>
#include <world/world_flat.h>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

int main() {
	using namespace vrl;

	auto &app = App::instance();
	app.init();
	app.set_flag_vsync(false);
	app.set_flag_depth_test(true);
	app.set_clear_color({ 0.0f, 0.0f, 0.0f });

	// prepare camera
	Camera camera;

	// geometry pass shader
	ShaderProgram::ShaderProgramBuilder geometry_pass_shader_program_builder("geometry_pass_shader_program");
	{
		Shader::ShaderBuilder vs_builder("geometry_pass_vs");
		auto vertex_shader = vs_builder.set_type(ShaderType::VERTEX_SHADER).set_source_from_file("assets/geometry.vert").build();

		Shader::ShaderBuilder fs_builder("geometry_pass_fs");
		auto fragment_shader = fs_builder.set_type(ShaderType::FRAGMENT_SHADER).set_source_from_file("assets/geometry.frag").build();

		geometry_pass_shader_program_builder.add_shader(vertex_shader).add_shader(fragment_shader);
	}
	auto geometry_pass_shader_program = geometry_pass_shader_program_builder.build();
	geometry_pass_shader_program.use();
	geometry_pass_shader_program.set_uniform("projection", camera.get_projection());

	// geometry pass
	auto albedo = Texture::TextureBuilder("albedo")
	                  .set_size(config::initial_window_width, config::initial_window_height)
	                  .set_format(GL_RGBA32F)
	                  .build();
	auto depth = Texture::TextureBuilder("depth")  // NOTE: internally use GL_NEAREST, that's actually ideal for depth -> they shouldn't be smoothed
	                 .set_size(config::initial_window_width, config::initial_window_height)
	                 .set_format(GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT)
	                 .build();
	auto geometry_pass = RenderPass::RenderPassBuilder("geometry_pass")
	                         .add_color_attachment(albedo)
	                         .set_depth_attachment(depth)
	                         .build();

	ShaderProgram::ShaderProgramBuilder blur_pass_shader_program_builder("blur_pass_shader_program");
	{
		Shader::ShaderBuilder vs_builder("blur_vs");
		auto vertex_shader = vs_builder
		                         .set_type(ShaderType::VERTEX_SHADER)
		                         .set_source_from_file("assets/blur.vert")
		                         .build();

		Shader::ShaderBuilder fs_builder("blur_fs");
		auto fragment_shader = fs_builder
		                           .set_type(ShaderType::FRAGMENT_SHADER)
		                           .set_source_from_file("assets/blur.frag")
		                           .build();
		blur_pass_shader_program_builder.add_shader(vertex_shader).add_shader(fragment_shader);
	}
	auto blur_pass_shader_program = blur_pass_shader_program_builder.build();

	auto quad_vertex_buffer = VertexBuffer::VertexBufferBuilder("fullscreen_quad", g_screen_quad_vertices)
	                              .add_attribute(2)  // position (vec2)
	                              .add_attribute(2)  // texture coordinates (vec2)
	                              .build();

	WorldFlat world_flat(10, 10, 10);
	world_flat.init();

	app.run([&](float dt) {
		camera.update(dt);

		geometry_pass.use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		app.set_flag_depth_test(true);
		geometry_pass_shader_program.use();
		geometry_pass_shader_program.set_uniform("view", camera.get_view());
		world_flat.render(geometry_pass_shader_program);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		app.set_flag_depth_test(false);
		albedo.use(0);
		quad_vertex_buffer.use();
		blur_pass_shader_program.use();
		blur_pass_shader_program.set_uniform("sampler", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	});

	app.shutdown();

	return 0;
}
