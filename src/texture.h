#pragma once

#include <glad/glad.h>

#include <optional>
#include <string>

#include <interface/build_target.h>
#include <interface/builder.h>

namespace vox {

struct TextureFormat {
	GLenum internal_format;  // e.g. GL_RGBA8
	GLenum pixel_format;     // e.g. GL_RGBA
	GLenum pixel_type;       // e.g. GL_UNSIGNED_BYTE
};

class Texture : public IBuildTarget<Texture> {
public:
	class TextureBuilder : public IBuilder<TextureBuilder, Texture> {
	public:
		TextureBuilder(const std::string &name);

		TextureBuilder &set_type(GLenum type);
		TextureBuilder &set_size(size_t width, size_t height);
		TextureBuilder &set_format(GLenum internal_format, GLenum pixel_format, GLenum pixel_type);
		TextureBuilder &set_filter(GLint filter);
		TextureBuilder &set_data_path(const std::string &path);

		[[nodiscard]] std::shared_ptr<Texture> _build() const;

	private:
		std::optional<GLenum> _type;
		std::optional<std::pair<size_t, size_t>> _size;
		std::optional<TextureFormat> _format;
		std::optional<GLint> _filter;
		std::optional<std::string> _data_path;
	};

	Texture() = default;
	virtual ~Texture();

	[[nodiscard]] GLuint get_handle() const;
	void bind_texture(GLuint bind_point) const;

protected:
	GLuint _texture;
};

}  // namespace vox
