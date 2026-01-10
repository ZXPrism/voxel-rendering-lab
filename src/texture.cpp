#include <texture.h>

#include <logger.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma clang diagnostic pop

namespace vox {

Texture::~Texture() {
	glDeleteTextures(1, &_texture);
}

Texture::Texture(const std::string &path) {
	int tex_width = 0;
	int tex_height = 0;
	int tex_n_channels = 0;
	auto *tex_src = stbi_load(path.c_str(), &tex_width, &tex_height, &tex_n_channels, 4);
	if (tex_src == nullptr) {
		SPDLOG_WARN("texture \"{}\" does not exist!", path);
		return;
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &_texture);
	glTextureParameteri(_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureStorage2D(_texture, 1, GL_SRGB8_ALPHA8, tex_width, tex_height);
	glTextureSubImage2D(_texture, 0, 0, 0, tex_width, tex_height, GL_RGBA, GL_UNSIGNED_BYTE, tex_src);

	stbi_image_free(tex_src);
}

void Texture::bind_texture(GLuint bind_point) const {
	glBindTextureUnit(bind_point, _texture);
}

}  // namespace vox
