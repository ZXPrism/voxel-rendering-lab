#include <world.h>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

namespace vox {

constexpr int WORLD_SIZE_LENGTH = 128;

static const Vertex cube_vertices[] = {
	// +X
	{ .pos = { 0.5f, -0.5f, -0.5f }, .normal = { 1, 0, 0 }, .uv = { 0, 1 } },
	{ .pos = { 0.5f, 0.5f, -0.5f }, .normal = { 1, 0, 0 }, .uv = { 0, 0 } },
	{ .pos = { 0.5f, 0.5f, 0.5f }, .normal = { 1, 0, 0 }, .uv = { 1, 0 } },
	{ .pos = { 0.5f, -0.5f, 0.5f }, .normal = { 1, 0, 0 }, .uv = { 1, 1 } },
	// -X
	{ .pos = { -0.5f, -0.5f, 0.5f }, .normal = { -1, 0, 0 }, .uv = { 0, 1 } },
	{ .pos = { -0.5f, 0.5f, 0.5f }, .normal = { -1, 0, 0 }, .uv = { 0, 0 } },
	{ .pos = { -0.5f, 0.5f, -0.5f }, .normal = { -1, 0, 0 }, .uv = { 1, 0 } },
	{ .pos = { -0.5f, -0.5f, -0.5f }, .normal = { -1, 0, 0 }, .uv = { 1, 1 } },
	// +Y
	{ .pos = { -0.5f, 0.5f, -0.5f }, .normal = { 0, 1, 0 }, .uv = { 0, 0 } },
	{ .pos = { -0.5f, 0.5f, 0.5f }, .normal = { 0, 1, 0 }, .uv = { 0, 1 } },
	{ .pos = { 0.5f, 0.5f, 0.5f }, .normal = { 0, 1, 0 }, .uv = { 1, 1 } },
	{ .pos = { 0.5f, 0.5f, -0.5f }, .normal = { 0, 1, 0 }, .uv = { 1, 0 } },
	// -Y
	{ .pos = { -0.5f, -0.5f, 0.5f }, .normal = { 0, -1, 0 }, .uv = { 0, 0 } },
	{ .pos = { -0.5f, -0.5f, -0.5f }, .normal = { 0, -1, 0 }, .uv = { 0, 1 } },
	{ .pos = { 0.5f, -0.5f, -0.5f }, .normal = { 0, -1, 0 }, .uv = { 1, 1 } },
	{ .pos = { 0.5f, -0.5f, 0.5f }, .normal = { 0, -1, 0 }, .uv = { 1, 0 } },
	// +Z
	{ .pos = { -0.5f, -0.5f, 0.5f }, .normal = { 0, 0, 1 }, .uv = { 1, 1 } },
	{ .pos = { 0.5f, -0.5f, 0.5f }, .normal = { 0, 0, 1 }, .uv = { 0, 1 } },
	{ .pos = { 0.5f, 0.5f, 0.5f }, .normal = { 0, 0, 1 }, .uv = { 0, 0 } },
	{ .pos = { -0.5f, 0.5f, 0.5f }, .normal = { 0, 0, 1 }, .uv = { 1, 0 } },
	// -Z
	{ .pos = { 0.5f, -0.5f, -0.5f }, .normal = { 0, 0, -1 }, .uv = { 1, 1 } },
	{ .pos = { -0.5f, -0.5f, -0.5f }, .normal = { 0, 0, -1 }, .uv = { 0, 1 } },
	{ .pos = { -0.5f, 0.5f, -0.5f }, .normal = { 0, 0, -1 }, .uv = { 0, 0 } },
	{ .pos = { 0.5f, 0.5f, -0.5f }, .normal = { 0, 0, -1 }, .uv = { 1, 0 } },
};

static const uint32_t cube_indices[] = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4,
	8, 9, 10, 10, 11, 8,
	12, 13, 14, 14, 15, 12,
	16, 17, 18, 18, 19, 16,
	20, 21, 22, 22, 23, 20
};

[[nodiscard]] Block World::get_block(int x, int y, int z) const {
	return _world_data[(x * WORLD_SIZE_LENGTH * WORLD_SIZE_LENGTH) + (y * WORLD_SIZE_LENGTH) + z];
}

[[nodiscard]] Block &World::get_block(int x, int y, int z) {
	return _world_data[(x * WORLD_SIZE_LENGTH * WORLD_SIZE_LENGTH) + (y * WORLD_SIZE_LENGTH) + z];
}

void World::_prep_storage() {
	// set up world data buffer
	_world_data.resize(WORLD_SIZE_LENGTH * WORLD_SIZE_LENGTH * WORLD_SIZE_LENGTH);

	// world prep
	for (int x = 0; x < WORLD_SIZE_LENGTH; x++) {
		for (int z = 0; z < WORLD_SIZE_LENGTH; z++) {
			auto t = stb_perlin_fbm_noise3(
			    static_cast<float>(x) * 0.1f, 0.0f, static_cast<float>(z) * 0.1f,
			    2.0f,
			    0.5f,
			    5);
			int height = static_cast<int>(t * 10.0f) + 10;

			for (int y = 0; y < height; y++) {
				auto &block = get_block(x, y, z);
				if (y <= 4) {
					block._type = BlockType::STONE;
				} else if (y <= 10) {
					block._type = BlockType::DIRT;
				} else {
					block._type = BlockType::GRASS;
				}
			}
			for (int y = height; y < WORLD_SIZE_LENGTH; y++) {
				auto &block = get_block(x, y, z);
				block._type = BlockType::AIR;
			}

			_voxel_cnt += height;
		}
	}
}

void World::_prep_render() {
	// VAO
	_vertex_array = std::make_unique<vox::VertexArray>();

	// VBO
	_voxel_data = std::make_unique<vox::ArrayBuffer>(
	    reinterpret_cast<const uint8_t *>(cube_vertices),
	    static_cast<int>(sizeof(cube_vertices)));

	// EBO
	_voxel_index_data = std::make_unique<vox::ArrayBuffer>(
	    reinterpret_cast<const uint8_t *>(cube_indices),
	    static_cast<int>(sizeof(cube_indices)));

	glVertexArrayVertexBuffer(_vertex_array->_handle, 0, _voxel_data->_handle, 0, sizeof(Vertex));
	glVertexArrayElementBuffer(_vertex_array->_handle, _voxel_index_data->_handle);

	glVertexArrayAttribBinding(_vertex_array->_handle, 0, 0);
	glVertexArrayAttribBinding(_vertex_array->_handle, 1, 0);
	glVertexArrayAttribBinding(_vertex_array->_handle, 2, 0);

	glEnableVertexArrayAttrib(_vertex_array->_handle, 0);  // pos
	glEnableVertexArrayAttrib(_vertex_array->_handle, 1);  // normal
	glEnableVertexArrayAttrib(_vertex_array->_handle, 2);  // uv

	glVertexArrayAttribFormat(_vertex_array->_handle, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
	glVertexArrayAttribFormat(_vertex_array->_handle, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	glVertexArrayAttribFormat(_vertex_array->_handle, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));

	// instancing VBO
	std::vector<Instance> instance_data;

	for (int x = 0; x < WORLD_SIZE_LENGTH; x++) {
		for (int z = 0; z < WORLD_SIZE_LENGTH; z++) {
			for (int y = 0; y < WORLD_SIZE_LENGTH; y++) {
				auto &block = get_block(x, y, z);

				int texture = 0;
				switch (block._type) {
				case BlockType::GRASS: texture = 0; break;
				case BlockType::DIRT: texture = 1; break;
				case BlockType::STONE: texture = 2; break;
				default: break;
				}

				if (block._type != BlockType::AIR) {
					instance_data.emplace_back(
					    glm::vec3{
					        static_cast<float>(x - (WORLD_SIZE_LENGTH / 2)),
					        static_cast<float>(y),
					        static_cast<float>(z - (WORLD_SIZE_LENGTH / 2)) },
					    texture);
				}
			}
		}
	}

	_instance_data = std::make_unique<vox::ArrayBuffer>(
	    reinterpret_cast<const uint8_t *>(instance_data.data()),
	    static_cast<int>(instance_data.size() * sizeof(Instance)));

	// register attribute
	glVertexArrayVertexBuffer(_vertex_array->_handle, 1, _instance_data->_handle, 0, sizeof(Instance));
	glVertexArrayBindingDivisor(_vertex_array->_handle, 1, 1);

	glVertexArrayAttribBinding(_vertex_array->_handle, 3, 1);
	glVertexArrayAttribFormat(_vertex_array->_handle, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Instance, translation));
	glEnableVertexArrayAttrib(_vertex_array->_handle, 3);

	glVertexArrayAttribBinding(_vertex_array->_handle, 4, 1);
	glVertexArrayAttribIFormat(_vertex_array->_handle, 4, 1, GL_INT, offsetof(Instance, texture));
	glEnableVertexArrayAttrib(_vertex_array->_handle, 4);
}

void World::generate() {
	_prep_storage();
	_prep_render();
}

void World::render() {
	_vertex_array->use();
	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr, _voxel_cnt);
}

}  // namespace vox
