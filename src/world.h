#pragma once

#include <glm/glm.hpp>

#include <vertex_array.h>

#include <memory>
#include <vector>

namespace vox {

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Instance {
	glm::vec3 translation;
	int texture;
};

class World {
public:
	void generate();
	void render();

private:
	std::unique_ptr<vox::VertexArray> _vertex_array;
	std::unique_ptr<vox::ArrayBuffer> _voxel_data;
	std::unique_ptr<vox::ArrayBuffer> _voxel_index_data;
	std::unique_ptr<vox::ArrayBuffer> _instance_data;

	int _voxel_cnt = 0;
	std::vector<std::vector<int>> _world_data;  // [x][z] = height
};

}  // namespace vox
