#pragma once

#include <glad/glad.h>

#include <string>

namespace vox {

class Texture {
public:
	Texture() = default;
	explicit Texture(const std::string &path);
	virtual ~Texture();

	[[nodiscard]] GLuint get_handle() const;
	void bind_texture(GLuint bind_point) const;

protected:
	GLuint _texture;
};

}  // namespace vox
