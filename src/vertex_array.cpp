#include <vertex_array.h>

namespace vox {

VertexArray::VertexArray() {
	glCreateVertexArrays(1, &_handle);
}

VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &_handle);
}

}  // namespace vox
