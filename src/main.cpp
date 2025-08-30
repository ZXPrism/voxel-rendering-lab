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

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
constexpr const char *WINDOW_TITLE = "voxel-rendering-lab [powered by gfx-utils]";

int main() {
	using namespace vrl;
	using namespace gfxutils;

	auto &app = App::instance();
	app.init(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
	app.set_flag_vsync(false);

	// prepare camera
	Camera camera;

	// geometry pass shader
	ShaderProgram::ShaderProgramBuilder geometry_pass_shader_program_builder("geometry_pass_shader_program");
	{
		Shader::ShaderBuilder vs_builder("geometry_pass_vs");
		auto vertex_shader = vs_builder.set_type(ShaderType::VERTEX_SHADER).set_source_from_file("assets/deferred/geometry.vert").build();

		Shader::ShaderBuilder fs_builder("geometry_pass_fs");
		auto fragment_shader = fs_builder.set_type(ShaderType::FRAGMENT_SHADER).set_source_from_file("assets/deferred/geometry.frag").build();

		geometry_pass_shader_program_builder.add_shader(vertex_shader).add_shader(fragment_shader);
	}
	auto geometry_pass_shader_program = geometry_pass_shader_program_builder.build();
	geometry_pass_shader_program.use();
	geometry_pass_shader_program.set_uniform("projection", camera.get_projection());

	// geometry pass
	auto albedo = Texture::TextureBuilder("albedo")
	                  .set_size(WINDOW_WIDTH, WINDOW_HEIGHT)
	                  .set_format(GL_RGB32F, GL_RGB)
	                  .build();
	auto position = Texture::TextureBuilder("position")
	                    .set_size(WINDOW_WIDTH, WINDOW_HEIGHT)
	                    .set_format(GL_RGB32F, GL_RGB)
	                    .build();
	auto normal = Texture::TextureBuilder("normal")
	                  .set_size(WINDOW_WIDTH, WINDOW_HEIGHT)
	                  .set_format(GL_RGB32F, GL_RGB)
	                  .build();
	auto depth = Texture::TextureBuilder("depth")
	                 .set_size(WINDOW_WIDTH, WINDOW_HEIGHT)
	                 .set_format(GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT)
	                 .build();
	auto geometry_pass = RenderPass::RenderPassBuilder("geometry_pass")
	                         .add_color_attachment(albedo, true)
	                         .add_color_attachment(position, false)
	                         .add_color_attachment(normal, true, glm::vec4(0.0f))
	                         .set_depth_attachment(depth)
	                         .build();

	// lighting pass shader
	ShaderProgram::ShaderProgramBuilder lighting_pass_shader_program_builder("lighting_pass_shader_program");
	{
		Shader::ShaderBuilder vs_builder("lighting_vs");
		auto vertex_shader = vs_builder
		                         .set_type(ShaderType::VERTEX_SHADER)
		                         .set_source_from_file("assets/deferred/lighting.vert")
		                         .build();

		Shader::ShaderBuilder fs_builder("lighting_fs");
		auto fragment_shader = fs_builder
		                           .set_type(ShaderType::FRAGMENT_SHADER)
		                           .set_source_from_file("assets/deferred/lighting.frag")
		                           .build();
		lighting_pass_shader_program_builder.add_shader(vertex_shader).add_shader(fragment_shader);
	}
	auto lighting_pass_shader_program = lighting_pass_shader_program_builder.build();
	lighting_pass_shader_program.use();
	lighting_pass_shader_program.set_uniform("window_width", static_cast<int>(WINDOW_WIDTH));
	lighting_pass_shader_program.set_uniform("window_height", static_cast<int>(WINDOW_HEIGHT));

	// lighting pass
	auto lighting_pass = RenderPass::RenderPassBuilder("lighting_pass").build();

	auto quad_vertex_buffer = VertexBuffer::VertexBufferBuilder("fullscreen_quad", g_screen_quad_vertices)
	                              .add_attribute(2)  // position (vec2)
	                              .add_attribute(2)  // texture coordinates (vec2)
	                              .build();

	WorldFlat world_flat(50, 12, 50);
	world_flat.init();

	int display_mode = 0;
	app.run([&](float dt) {
		camera.update(dt);

		ImGui::Begin("Control", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
		{
			const char *items[] = { "final", "albedo", "position", "normal", "all" };
			ImGui::Combo("Color", &display_mode, items, IM_ARRAYSIZE(items));
		}
		ImGui::End();

		geometry_pass.use(true, [&]() {
			geometry_pass_shader_program.use();
			geometry_pass_shader_program.set_uniform("view", camera.get_view());

			world_flat.render(geometry_pass_shader_program);
		});

		lighting_pass.use(false, [&]() {
			albedo.use(0);
			position.use(1);
			normal.use(2);

			lighting_pass_shader_program.use();
			lighting_pass_shader_program.set_uniform("sampler_albedo", 0);
			lighting_pass_shader_program.set_uniform("sampler_position", 1);
			lighting_pass_shader_program.set_uniform("sampler_normal", 2);
			lighting_pass_shader_program.set_uniform("light_position", camera.get_pos());
			lighting_pass_shader_program.set_uniform("light_color", glm::normalize(glm::abs(camera.get_pos())));
			lighting_pass_shader_program.set_uniform("u_display_mode", display_mode);

			quad_vertex_buffer.use();
			glDrawArrays(GL_TRIANGLES, 0, 6);
		});
	});

	app.shutdown();

	return 0;
}
