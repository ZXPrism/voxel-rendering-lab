// adapted from my own library <gfx-utils>

#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <interface/build_target.h>
#include <interface/builder.h>
#include <texture.h>

namespace vox {

struct RenderPassConfig {
	bool _enable_depth_test;
	bool _enable_srgb;
};

class RenderPass : public IBuildTarget<RenderPass> {
private:
	bool _IsDefault;  // i.e. use default FBO

	std::shared_ptr<GLuint> _FBO;
	std::vector<Texture> _color_attachments;
	std::vector<bool> _color_attachment_clear_flags;
	std::vector<glm::vec4> _color_attachment_clear_values;
	std::optional<Texture> _depth_attachment;

public:
	class RenderPassBuilder : public IBuilder<RenderPassBuilder, RenderPass> {
	private:
		std::vector<Texture> _color_attachments;
		std::vector<bool> _color_attachment_clear_flags;
		std::vector<glm::vec4> _color_attachment_clear_values;
		std::optional<Texture> _depth_attachment;

	public:
		RenderPassBuilder(const std::string &name);

		// NOTE: current only support float types
		// NOTE: if attachment's one component has less than 4 floats, only clear_value_rgba.r is used to clear it
		RenderPassBuilder &add_color_attachment(const Texture &texture, bool clear_before_use, const glm::vec4 &clear_value_rgba = {});
		RenderPassBuilder &set_depth_attachment(const Texture &texture);

		[[nodiscard]] RenderPass _build() const;
	};

	// NOTE: depth_test flag is ONLY meaningful when writing to default FBO
	// depth attachment(if any) will always be cleared regardless of this flag
	void use(const RenderPassConfig &render_pass_config, const std::function<void()> &callback) const;

private:
	RenderPass() = default;
};

}  // namespace vox
