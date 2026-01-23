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

public:
	class RenderPassBuilder : public IBuilder<RenderPassBuilder, RenderPass> {
	public:
		RenderPassBuilder(const std::string &name);

		// NOTE: current only support float types
		// NOTE: if attachment's one component has less than 4 floats, only clear_value_rgba.r is used to clear it
		RenderPassBuilder &add_color_attachment(const std::shared_ptr<Texture> &texture, bool clear_before_use, const glm::vec4 &clear_value_rgba = {});
		RenderPassBuilder &set_depth_attachment(const std::shared_ptr<Texture> &texture);

		[[nodiscard]] std::shared_ptr<RenderPass> _build() const;

	private:
		std::vector<std::shared_ptr<Texture>> _color_attachments;
		std::vector<bool> _color_attachment_clear_flags;
		std::vector<glm::vec4> _color_attachment_clear_values;
		std::optional<std::shared_ptr<Texture>> _depth_attachment;
	};

	// NOTE: depth_test flag is ONLY meaningful when writing to default FBO
	// depth attachment(if any) will always be cleared regardless of this flag
	void use(const RenderPassConfig &render_pass_config, const std::function<void()> &callback) const;

private:
	bool _IsDefault;  // i.e. use default FBO

	std::shared_ptr<GLuint> _FBO;
	std::vector<std::shared_ptr<Texture>> _color_attachments;
	std::vector<bool> _color_attachment_clear_flags;
	std::vector<glm::vec4> _color_attachment_clear_values;
	std::optional<std::shared_ptr<Texture>> _depth_attachment;
};

}  // namespace vox
