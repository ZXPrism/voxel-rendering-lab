#pragma once

#include <glad/glad.h>

#include <string>

namespace vox {

class Texture {
public:
	explicit Texture(const std::string &path);
	~Texture();

	void bind_texture(GLuint bind_point) const;

private:
	GLuint _texture;
};

}  // namespace vox
