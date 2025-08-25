#include <texture.h>

#include <logger.h>

namespace vrl {

Texture::TextureBuilder::TextureBuilder(const std::string &name)
    : IBuilder(name) {
}

Texture::TextureBuilder &Texture::TextureBuilder::set_size(size_t width, size_t height) {
	_Width = width;
	_Height = height;
	return *this;
}

Texture::TextureBuilder &Texture::TextureBuilder::set_format(GLenum internal_format, GLenum cpu_format, GLenum cpu_comp_type) {
	_InternalFormat = internal_format;
	_CPUFormat = cpu_format;
	_CPUCompType = cpu_comp_type;
	return *this;
}

Texture Texture::TextureBuilder::_build() {
	Texture res;

	auto texture_raw_handle = new GLuint(0);
	res._TextureHandle = std::shared_ptr<GLuint>(texture_raw_handle, [&](GLuint *ptr) {
		glDeleteTextures(1, ptr);
		delete ptr;
	});

	glGenTextures(1, res._TextureHandle.get());
	glBindTexture(GL_TEXTURE_2D, *res._TextureHandle);
	glTexImage2D(GL_TEXTURE_2D,
	             0,
	             _InternalFormat,
	             static_cast<GLsizei>(_Width),
	             static_cast<GLsizei>(_Height),
	             0,
	             _CPUFormat,
	             _CPUCompType,
	             nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	g_logger->info("Texture::TextureBuilder ({}): successfully built texture", _Name);

	res._set_complete();

	return res;
}

GLuint Texture::_get_handle() const {
	return *_TextureHandle;
}

void Texture::use(size_t location) {
	glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + location));
	glBindTexture(GL_TEXTURE_2D, *_TextureHandle);
}

}  // namespace vrl
