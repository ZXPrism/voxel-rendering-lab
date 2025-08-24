#pragma once

#include <interfaces/builder.h>
#include <texture.h>

#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>

namespace vrl {

class RenderPass {
private:
	std::shared_ptr<GLuint> _FBO;
	std::vector<Texture> _ColorAttachments;

public:
	class RenderPassBuilder : public IBuilder<RenderPassBuilder, RenderPass> {
	private:
		std::vector<Texture> _ColorAttachments;

	public:
		RenderPassBuilder(const std::string &name);

		RenderPassBuilder &add_color_attachment(const Texture &texture);

		RenderPass _build();
	};

	void use();

private:
	RenderPass() = default;
};

}  // namespace vrl
