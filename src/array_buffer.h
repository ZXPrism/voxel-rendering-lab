#pragma once

#include <glad/glad.h>

#include <cstdint>

namespace vox {

class ArrayBuffer {
public:
	GLuint _handle;

	explicit ArrayBuffer(const uint8_t *data, int n_bytes, GLbitfield usage);
	~ArrayBuffer();
};

}  // namespace vox
