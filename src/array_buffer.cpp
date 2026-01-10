#include <array_buffer.h>

namespace vox {

ArrayBuffer::ArrayBuffer(const uint8_t *data, int n_bytes) {
	glCreateBuffers(1, &_handle);

	// TODO: for maximum perf we may need to tweak buffer flags
	glNamedBufferStorage(_handle, n_bytes, data, GL_DYNAMIC_STORAGE_BIT);
}

ArrayBuffer::~ArrayBuffer() {
	glDeleteBuffers(1, &_handle);
}

}  // namespace vox
