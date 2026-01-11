#pragma once

#include <glm/glm.hpp>

#include <block.h>
#include <vertex_array.h>
#include <vertices.h>

#include <memory>
#include <vector>

namespace vox {

class World {
public:
	void generate();
	void render();

	[[nodiscard]] Block get_block(int x, int y, int z) const;
	[[nodiscard]] Block &get_block(int x, int y, int z);

private:
	void _prep_storage();
	void _prep_render();

	std::unique_ptr<vox::VertexArray> _vertex_array;
	std::unique_ptr<vox::ArrayBuffer> _voxel_data;
	std::unique_ptr<vox::ArrayBuffer> _voxel_index_data;
	std::unique_ptr<vox::ArrayBuffer> _instance_data;

	int _voxel_cnt = 0;
	std::vector<Block> _world_data;
};

}  // namespace vox
