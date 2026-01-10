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

void World::generate() {
	// render prep
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

	// world prep
	_world_data = std::vector<std::vector<int>>(WORLD_SIZE_LENGTH, std::vector<int>(WORLD_SIZE_LENGTH));
	for (int x = 0; x < WORLD_SIZE_LENGTH; x++) {
		for (int z = 0; z < WORLD_SIZE_LENGTH; z++) {
			int x_shifted = x - (WORLD_SIZE_LENGTH / 2);
			int z_shifted = z - (WORLD_SIZE_LENGTH / 2);
			auto t = stb_perlin_fbm_noise3(
			    static_cast<float>(x_shifted) * 0.1f, 0.0f, static_cast<float>(z_shifted) * 0.1f,
			    2.0f,
			    0.5f,
			    5);
			int height = static_cast<int>(t * 10.0f) + 10;
			_world_data[x][z] = height;

			_voxel_cnt += height;
		}
	}

	// instancing VBO
	const int voxel_cnt = WORLD_SIZE_LENGTH * WORLD_SIZE_LENGTH;
	std::vector<Instance> instance_data;
	instance_data.reserve(voxel_cnt);

	for (int x = 0; x < WORLD_SIZE_LENGTH; x++) {
		for (int z = 0; z < WORLD_SIZE_LENGTH; z++) {
			int x_shifted = x - (WORLD_SIZE_LENGTH / 2);
			int z_shifted = z - (WORLD_SIZE_LENGTH / 2);
			int height = _world_data[x][z];
			for (int y = 0; y < height; y++) {
				int texture = 0;
				if (y <= 4) {
					texture = 2;
				} else if (y <= 10) {
					texture = 1;
				} else {
					texture = 0;
				}

				instance_data.emplace_back(
				    glm::vec3{
				        static_cast<float>(x_shifted),
				        static_cast<float>(y),
				        static_cast<float>(z_shifted) },
				    texture);
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

void World::render() {
	_vertex_array->use();
	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr, _voxel_cnt);
}

}  // namespace vox
