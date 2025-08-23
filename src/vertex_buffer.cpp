#include <vertex_buffer.h>

#include <numeric>

#include <logger.h>

namespace vrl {

VertexBuffer::VertexBufferBuilder::VertexBufferBuilder(const std::string &name, const std::vector<float> &data)
    : IBuilder(name)
    , _Data(data) {
}

VertexBuffer::VertexBufferBuilder &VertexBuffer::VertexBufferBuilder::add_attribute(size_t n_bytes) {
	_AttrSizes.push_back(n_bytes);
	return *this;
}

VertexBuffer VertexBuffer::VertexBufferBuilder::_build() const {
	VertexBuffer res;

	GLuint *raw_VAO_handle = new GLuint(0);
	glGenVertexArrays(1, raw_VAO_handle);
	res._VAO = std::shared_ptr<GLuint>(raw_VAO_handle, [](GLuint *ptr) {
		glDeleteVertexArrays(1, ptr);
		delete ptr;
	});
	glBindVertexArray(*res._VAO);

	GLuint *raw_VBO_handle = new GLuint(0);
	glGenBuffers(1, raw_VBO_handle);
	res._VBO = std::shared_ptr<GLuint>(raw_VBO_handle, [](GLuint *ptr) {
		glDeleteBuffers(1, ptr);
		delete ptr;
	});
	glBindBuffer(GL_ARRAY_BUFFER, *res._VBO);

	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(_Data.size() * sizeof(float)), _Data.data(), GL_STATIC_DRAW);

	size_t n_attrs = _AttrSizes.size();
	size_t stride_bytes = std::accumulate(_AttrSizes.begin(), _AttrSizes.end(), static_cast<size_t>(0)) * sizeof(float);
	for (size_t i = 0, offset_bytes = 0; i < n_attrs; i++) {
		glVertexAttribPointer(static_cast<GLuint>(i),
		                      static_cast<GLint>(_AttrSizes[i]),
		                      GL_FLOAT,
		                      GL_FALSE,
		                      static_cast<GLsizei>(stride_bytes),
		                      reinterpret_cast<const void *>(offset_bytes));
		glEnableVertexAttribArray(static_cast<GLuint>(i));
		offset_bytes += _AttrSizes[i] * sizeof(float);

		g_logger->info("VertexBuffer::VertexBufferBuilder ({}): enabled vertex attribute {} with {} components", _Name, i, _AttrSizes[i]);
	}

	glBindVertexArray(0);

	return res;
}

void VertexBuffer::use() const {
	glBindVertexArray(*_VAO);
}

}  // namespace vrl
