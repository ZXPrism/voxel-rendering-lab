#include <texture.h>

#include <logger.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma clang diagnostic pop

namespace vox {

// =================
//  Texture Builder
// =================

Texture::TextureBuilder::TextureBuilder(const std::string &name)
    : IBuilder(name) {
}

Texture::TextureBuilder &Texture::TextureBuilder::set_size(size_t width, size_t height) {
	_size = { width, height };
	return *this;
}

Texture::TextureBuilder &Texture::TextureBuilder::set_type(GLenum type) {
	_type = type;
	return *this;
}

Texture::TextureBuilder &Texture::TextureBuilder::set_format(GLenum internal_format, GLenum pixel_format, GLenum pixel_type) {
	_format = TextureFormat{
		.internal_format = internal_format,
		.pixel_format = pixel_format,
		.pixel_type = pixel_type
	};
	return *this;
}

Texture::TextureBuilder &Texture::TextureBuilder::set_filter(GLint filter) {
	_filter = filter;
	return *this;
}

Texture::TextureBuilder &Texture::TextureBuilder::set_data_path(const std::string &path) {
	_data_path = path;
	return *this;
}

std::shared_ptr<Texture> Texture::TextureBuilder::_build() const {
	auto res = std::make_shared<Texture>();

	res->_set_name(_name);

	if (_data_path.has_value() && _size.has_value()) {
		SPDLOG_WARN("Texture::TextureBuilder ({}): can not set texture size and data path at the same time: texture won't be built", _name);
		return res;
	}

	if (!_data_path.has_value() && !_size.has_value()) {
		SPDLOG_WARN("Texture::TextureBuilder ({}): neither texture size nor data path is set: texture won't be built", _name);
		return res;
	}

	if (!_type.has_value()) {
		SPDLOG_WARN("Texture::TextureBuilder ({}): texture type is not set: texture won't be built", _name);
		return res;
	}

	if (!_format.has_value()) {
		SPDLOG_WARN("Texture::TextureBuilder ({}): texture format is not set: texture won't be built", _name);
		return res;
	}

	if (!_filter.has_value()) {
		SPDLOG_WARN("Texture::TextureBuilder ({}): texture filter is not set: texture won't be built", _name);
		return res;
	}

	glCreateTextures(*_type, 1, &res->_texture);
	glTextureParameteri(res->_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(res->_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(res->_texture, GL_TEXTURE_MIN_FILTER, *_filter);
	glTextureParameteri(res->_texture, GL_TEXTURE_MAG_FILTER, *_filter);

	if (_data_path.has_value()) {
		if (_type == GL_TEXTURE_2D) {
			int tex_width = 0;
			int tex_height = 0;
			int tex_n_channels = 0;
			auto *tex_src = stbi_load(_data_path->c_str(), &tex_width, &tex_height, &tex_n_channels, 4);
			if (tex_src == nullptr) {
				SPDLOG_WARN("Texture::TextureBuilder ({}): path \"{}\" invalid, texture won't be built", _name, *_data_path);
				return res;
			}

			glTextureStorage2D(res->_texture, 1, _format->internal_format, tex_width, tex_height);
			glTextureSubImage2D(res->_texture, 0, 0, 0, tex_width, tex_height, _format->pixel_format, _format->pixel_type, tex_src);

			stbi_image_free(tex_src);
		} else if (_type == GL_TEXTURE_CUBE_MAP) {
			static const std::string filename_list[6]{ "px.png", "nx.png", "py.png", "ny.png", "pz.png", "nz.png" };

			std::optional<std::pair<int, int>> tex_size;

			for (int i = 0; i < 6; i++) {
				const std::string face_path = std::format("{}/{}", *_data_path, filename_list[i]);

				int tex_width = 0;
				int tex_height = 0;
				int tex_n_channels = 0;
				auto *tex_src = stbi_load(face_path.c_str(), &tex_width, &tex_height, &tex_n_channels, 4);
				if (tex_src == nullptr) {
					SPDLOG_WARN("Texture::TextureBuilder ({}): path \"{}\" invalid, texture won't be built", _name, face_path);
					return res;
				}

				if (!tex_size.has_value()) {
					tex_size = { tex_width, tex_height };  // FIX formats!
					glTextureStorage2D(res->_texture, 1, _format->internal_format, tex_width, tex_height);
				} else if (tex_size->first != tex_width || tex_size->second != tex_height) {
					SPDLOG_WARN("Texture::TextureBuilder ({}): cubemap face texture sizes are not all equal, texture won't be built", _name);
					return res;
				}

				glTextureSubImage3D(
				    res->_texture,
				    0,
				    0, 0, i,
				    tex_size->first, tex_size->second, 1,
				    _format->pixel_format,
				    _format->pixel_type,
				    tex_src);

				stbi_image_free(tex_src);
			}
		} else {
			SPDLOG_WARN("Texture::TextureBuilder ({}): unsupported texture gpu format {}, texture won't be built", _name, *_type);
			return res;
		}
	} else {
		glTextureStorage2D(res->_texture, 1, _format->internal_format, static_cast<GLsizei>(_size->first), static_cast<GLsizei>(_size->second));
	}

	SPDLOG_INFO("Texture::TextureBuilder ({}): successfully built texture", _name);

	res->_set_complete();

	return res;
}

// =========
//  Texture
// =========

Texture::~Texture() {
	glDeleteTextures(1, &_texture);
}

GLuint Texture::get_handle() const {
	return _texture;
}

void Texture::bind_texture(GLuint bind_point) const {
	glBindTextureUnit(bind_point, _texture);
}

}  // namespace vox
