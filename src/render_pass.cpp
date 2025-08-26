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

RenderPass::RenderPassBuilder &RenderPass::RenderPassBuilder::set_depth_attachment(const Texture &texture) {
	_DepthAttachment = texture;
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

	if (!n_color_attachments && !_DepthAttachment.has_value()) {  // no attachments, fallback to default FBO
		return res;
	}

	glGenFramebuffers(1, res._FBO.get());
	glBindFramebuffer(GL_FRAMEBUFFER, *res._FBO);

	std::vector<GLenum> attachments(n_color_attachments);

	for (size_t i = 0; i < n_color_attachments; i++) {
		auto texture_handle = res._ColorAttachments[i]._get_handle();
		attachments[i] = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i);
		if (res._ColorAttachments[i].is_complete()) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, texture_handle, 0);
		}
	}

	if (_DepthAttachment.has_value()) {
		res._DepthAttachment = _DepthAttachment.value();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, res._DepthAttachment._get_handle(), 0);
	}

	glDrawBuffers(static_cast<GLsizei>(n_color_attachments), attachments.data());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		g_logger->warn("RenderPass::RenderPassBuilder ({}): framebuffer is not complete!", _Name);
		return res;
	}

	g_logger->info("RenderPass::RenderPassBuilder ({}): successfully built render pass with {} color attachment(s)", _Name, n_color_attachments);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	res._set_complete();

	return res;
}

void RenderPass::clear(int mask) const {
	glClear(mask);
}

void RenderPass::use(const std::function<void()> &callback) const {
	glBindFramebuffer(GL_FRAMEBUFFER, *_FBO);
	callback();
}

}  // namespace vrl
