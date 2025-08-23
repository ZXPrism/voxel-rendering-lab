#pragma once

namespace vrl {

enum class ShaderType {
	UNKNOWN,
	VERTEX_SHADER,
	FRAGMENT_SHADER,
	COMPUTE_SHADER,
};

enum class ShaderDataType {
	INT,
	FLOAT,
	VEC2,
	VEC3,
	VEC4,
	MAT2,
	MAT3,
	MAT4,
};

}  // namespace vrl
