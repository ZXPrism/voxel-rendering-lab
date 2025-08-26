#pragma once

#include <interfaces/build_target.h>
#include <interfaces/builder.h>
#include <texture.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <glad/glad.h>

namespace vrl {

class RenderPass : public IBuildTarget<RenderPass> {
private:
	std::shared_ptr<GLuint> _FBO;
	std::vector<Texture> _ColorAttachments;
	Texture _DepthAttachment;

public:
	class RenderPassBuilder : public IBuilder<RenderPassBuilder, RenderPass> {
	private:
		std::vector<Texture> _ColorAttachments;
		std::optional<Texture> _DepthAttachment;

	public:
		RenderPassBuilder(const std::string &name);

		RenderPassBuilder &add_color_attachment(const Texture &texture);
		RenderPassBuilder &set_depth_attachment(const Texture &texture);

		RenderPass _build();
	};

	void clear(int mask) const;
	void use(const std::function<void()> &callback) const;

private:
	RenderPass() = default;
};

}  // namespace vrl
