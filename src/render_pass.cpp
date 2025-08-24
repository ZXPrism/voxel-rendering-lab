#include <render_pass.h>

#include <logger.h>

namespace vrl {

RenderPass::RenderPassBuilder::RenderPassBuilder(const std::string &name)
    : IBuilder(name) {
}

RenderPass::RenderPassBuilder &RenderPass::RenderPassBuilder::add_color_attachment(const Texture &texture) {
	_ColorAttachments.push_back(texture);
	return *this;
}

RenderPass RenderPass::RenderPassBuilder::_build() {
	RenderPass res;

	size_t n_color_attachments = _ColorAttachments.size();
	res._ColorAttachments = _ColorAttachments;

	auto fbo_raw_handle = new GLuint(0);
	res._FBO = std::shared_ptr<GLuint>(fbo_raw_handle, [&](GLuint *ptr) {
		glDeleteFramebuffers(1, ptr);
		delete ptr;
	});

	glGenFramebuffers(1, res._FBO.get());
	glBindFramebuffer(GL_FRAMEBUFFER, *res._FBO);

	std::vector<GLenum> attachments(n_color_attachments);

	for (size_t i = 0; i < n_color_attachments; i++) {
		auto texture_handle = res._ColorAttachments[i]._get_handle();
		attachments[i] = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, texture_handle, 0);
	}

	glDrawBuffers(static_cast<GLsizei>(n_color_attachments), attachments.data());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		g_logger->warn("RenderPass::RenderPassBuilder ({}): framebuffer is not complete!", _Name);
		return res;
	}

	g_logger->info("RenderPass::RenderPassBuilder ({}): successfully built render pass with {} color attachment(s)", _Name, n_color_attachments);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return res;
}

void RenderPass::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, *_FBO);
}

}  // namespace vrl
