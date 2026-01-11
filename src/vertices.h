#pragma once

#include <glm/glm.hpp>

namespace vox {

struct SkyboxVertex {
	glm::vec3 pos;
};

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Instance {
	glm::vec3 translation;
	int texture;
};

extern const SkyboxVertex skybox_vertices[36];
extern const Vertex cube_vertices[24];
extern const uint32_t cube_indices[36];

}  // namespace vox
