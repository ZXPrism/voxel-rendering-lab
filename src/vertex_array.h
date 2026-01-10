#pragma once

#include <glad/glad.h>

#include <array_buffer.h>

namespace vox {

class VertexArray {
public:
	VertexArray();
	~VertexArray();

	void use() const;

	GLuint _handle;

private:
};

}  // namespace vox
