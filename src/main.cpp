#include <world/world_flat.h>

#include <gfx-utils-core/app.h>
#include <gfx-utils-core/camera.h>
#include <gfx-utils-core/logger.h>
#include <gfx-utils-core/render_pass.h>
#include <gfx-utils-core/shader_program.h>
#include <gfx-utils-core/vertices.h>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr const char *WINDOW_TITLE = "voxel-rendering-lab";

int main() {
	using namespace vrl;
	using namespace gfxutils;

	auto &app = App::instance();
	app.init(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
	app.set_flag_vsync(false);
	app.set_clear_color({ 0.341f, 0.808f, 0.980f });

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
	                  .set_size(WINDOW_WIDTH, WINDOW_HEIGHT)
	                  .set_format(GL_RGBA32F)
	                  .build();
	auto depth = Texture::TextureBuilder("depth")  // NOTE: internally use GL_NEAREST, that's actually ideal for depth -> they shouldn't be smoothed
	                 .set_size(WINDOW_WIDTH, WINDOW_HEIGHT)
	                 .set_format(GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT)
	                 .build();
	auto geometry_pass = RenderPass::RenderPassBuilder("geometry_pass")
	                         .add_color_attachment(albedo, true)
	                         .set_depth_attachment(depth)
	                         .build();

	// fx pass
	auto fx_pass = RenderPass::RenderPassBuilder("fx_pass").build();

	ShaderProgram::ShaderProgramBuilder fx_pass_shader_program_builder("fx_pass_shader_program");
	{
		Shader::ShaderBuilder vs_builder("fx_vs");
		auto vertex_shader = vs_builder
		                         .set_type(ShaderType::VERTEX_SHADER)
		                         .set_source_from_file("assets/fx.vert")
		                         .build();

		Shader::ShaderBuilder fs_builder("fx_fs");
		auto fragment_shader = fs_builder
		                           .set_type(ShaderType::FRAGMENT_SHADER)
		                           .set_source_from_file("assets/fx.frag")
		                           .build();
		fx_pass_shader_program_builder.add_shader(vertex_shader).add_shader(fragment_shader);
	}
	auto fx_pass_shader_program = fx_pass_shader_program_builder.build();

	auto quad_vertex_buffer = VertexBuffer::VertexBufferBuilder("fullscreen_quad", g_screen_quad_vertices)
	                              .add_attribute(2)  // position (vec2)
	                              .add_attribute(2)  // texture coordinates (vec2)
	                              .build();

	WorldFlat world_flat(10, 10, 10);
	world_flat.init();

	app.run([&](float dt) {
		camera.update(dt);

		geometry_pass.use(true, [&]() {
			geometry_pass_shader_program.use();
			geometry_pass_shader_program.set_uniform("view", camera.get_view());
			world_flat.render(geometry_pass_shader_program);
		});

		fx_pass.use(false, [&]() {
			albedo.use(0);
			quad_vertex_buffer.use();
			fx_pass_shader_program.use();
			fx_pass_shader_program.set_uniform("sampler", 0);
			fx_pass_shader_program.set_uniform("window_width", WINDOW_WIDTH);
			fx_pass_shader_program.set_uniform("window_height", WINDOW_HEIGHT);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		});
	});

	app.shutdown();

	return 0;
}
