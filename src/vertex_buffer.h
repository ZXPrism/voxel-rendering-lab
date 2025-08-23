#pragma once

#include <interfaces/builder.h>

#include <glad/glad.h>

#include <memory>
#include <vector>

namespace vrl {

class VertexBuffer {
private:
	std::shared_ptr<GLuint> _VAO;
	std::shared_ptr<GLuint> _VBO;

public:
	class VertexBufferBuilder : public IBuilder<VertexBufferBuilder, VertexBuffer> {
	private:
		const std::vector<float> &_Data;
		std::vector<size_t> _AttrSizes;

	public:
		VertexBufferBuilder(const std::string &name, const std::vector<float> &data);

		VertexBufferBuilder &add_attribute(size_t n_bytes);

		VertexBuffer _build() const;
	};

	void use() const;

private:
	VertexBuffer() = default;
};

}  // namespace vrl
