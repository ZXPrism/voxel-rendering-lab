// adapted from my own library <gfx-utils>

#include <render_pass.h>

#include <logger.h>

namespace vox {

RenderPass::RenderPassBuilder::RenderPassBuilder(const std::string &name)
    : IBuilder(name) {
}

RenderPass::RenderPassBuilder &RenderPass::RenderPassBuilder::add_color_attachment(const Texture &texture, bool clear_before_use, const glm::vec4 &clear_value_rgba) {
	_color_attachments.push_back(texture);
	_color_attachment_clear_flags.push_back(clear_before_use);
	_color_attachment_clear_values.push_back(clear_value_rgba);
	return *this;
}

RenderPass::RenderPassBuilder &RenderPass::RenderPassBuilder::set_depth_attachment(const Texture &texture) {
	_depth_attachment = texture;
	return *this;
}

RenderPass RenderPass::RenderPassBuilder::_build() const {
	RenderPass res;

	res._set_name(_name);

	size_t n_color_attachments = _color_attachments.size();
	res._color_attachments = _color_attachments;
	res._color_attachment_clear_flags = _color_attachment_clear_flags;
	res._color_attachment_clear_values = _color_attachment_clear_values;

	res._IsDefault = n_color_attachments == 0 && !_depth_attachment.has_value();  // no attachments, fallback to default FBO

	auto *fbo_raw_handle = new GLuint(0);
	res._FBO = std::shared_ptr<GLuint>(fbo_raw_handle, [=](GLuint *ptr) {
		if (!res._IsDefault) {  // if fallback, the delete is handled by the window system, no need to manually delete it
			glDeleteFramebuffers(1, ptr);
		}
		delete ptr;
	});

	if (res._IsDefault) {
		SPDLOG_INFO("RenderPass::RenderPassBuilder ({}): successfully built default render pass", _name);
		return res;
	}

	glGenFramebuffers(1, res._FBO.get());
	glBindFramebuffer(GL_FRAMEBUFFER, *res._FBO);

	std::vector<GLenum> attachments(n_color_attachments);

	for (size_t i = 0; i < n_color_attachments; i++) {
		auto texture_handle = res._color_attachments[i].get_handle();
		attachments[i] = static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + i);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, texture_handle, 0);
	}

	if (_depth_attachment.has_value()) {
		res._depth_attachment = _depth_attachment;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, res._depth_attachment->get_handle(), 0);
	}

	glDrawBuffers(static_cast<GLsizei>(n_color_attachments), attachments.data());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		SPDLOG_WARN("RenderPass::RenderPassBuilder ({}): framebuffer is not complete!", _name);
		return res;
	}

	SPDLOG_INFO("RenderPass::RenderPassBuilder ({}): successfully built render pass with {} color attachment(s)", _name, n_color_attachments);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	res._set_complete();

	return res;
}

void RenderPass::use(const RenderPassConfig &render_pass_config, const std::function<void()> &callback) const {
	glBindFramebuffer(GL_FRAMEBUFFER, *_FBO);

	if (render_pass_config._enable_srgb) {
		glEnable(GL_FRAMEBUFFER_SRGB);
	} else {
		glDisable(GL_FRAMEBUFFER_SRGB);
	}

	if (render_pass_config._enable_depth_test) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}

	if (_IsDefault) {
		if (render_pass_config._enable_depth_test) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		} else {
			glClear(GL_COLOR_BUFFER_BIT);
		}
	} else {
		if (render_pass_config._enable_depth_test && _depth_attachment.has_value()) {
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		size_t n_color_attachments = _color_attachments.size();
		for (size_t i = 0; i < n_color_attachments; i++) {
			if (_color_attachment_clear_flags[i]) {
				glClearBufferfv(GL_COLOR, static_cast<GLint>(i), &_color_attachment_clear_values[i].r);
			}
		}
	}

	callback();
}

}  // namespace vox
