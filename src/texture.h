#pragma once

#include <interfaces/build_target.h>
#include <interfaces/builder.h>

#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>

namespace vrl {

enum class TextureFormat {
	RGB16F,
	RGB32F,
	// todo
};

class Texture : public IBuildTarget<Texture> {
private:
	std::shared_ptr<GLuint> _TextureHandle;

public:
	class TextureBuilder : public IBuilder<TextureBuilder, Texture> {
	private:
		std::string _SourcePath;  // todo
		size_t _Width;
		size_t _Height;
		GLenum _InternalFormat;
		GLenum _CPUFormat;
		GLenum _CPUCompType;

	public:
		TextureBuilder(const std::string &name);

		TextureBuilder &set_size(size_t width, size_t height);
		TextureBuilder &set_format(GLenum internal_format, GLenum cpu_format = GL_RGBA, GLenum cpu_comp_type = GL_FLOAT);

		Texture _build();
	};

	GLuint _get_handle() const;
	void use(size_t location);
};

}  // namespace vrl
