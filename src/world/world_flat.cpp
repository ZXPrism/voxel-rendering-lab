#include <world/world_flat.h>

#include <vertices.h>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace vrl {

WorldFlat::WorldFlat(size_t extent_x, size_t extent_y, size_t extent_z)
    : IWorld(extent_x, extent_y, extent_z)
    , _VertexBuffer(VertexBuffer::VertexBufferBuilder("vert builder", g_cube_vertices)
                        .add_attribute(3)  // pos
                        .add_attribute(3)  // normal
                        .add_attribute(2)  // uv
                        .build()) {
	_WorldData.resize(extent_x * extent_y * extent_z);
}

Voxel &WorldFlat::_get_voxel(size_t x, size_t y, size_t z) {
	return _WorldData[x * _ExtentY * _ExtentZ + y * _ExtentZ + z];
}

const Voxel &WorldFlat::_get_voxel(size_t x, size_t y, size_t z) const {
	return _WorldData[x * _ExtentY * _ExtentZ + y * _ExtentZ + z];
}

void WorldFlat::_render(ShaderProgram &shader_program) const {
	_VertexBuffer.use();

	for (size_t x = 0; x < _ExtentX; x++) {
		for (size_t y = 0; y < _ExtentY; y++) {
			for (size_t z = 0; z < _ExtentZ; z++) {
				glm::mat4 model(1.0f);
				model = glm::translate(model, glm::vec3{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) });
				shader_program.set_uniform("model", model);

				auto &voxel = _get_voxel(x, y, z);
				shader_program.set_uniform("type", static_cast<int>(voxel.type));

				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
	}
}

}  // namespace vrl
