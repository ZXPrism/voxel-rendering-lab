#include <world/world_flat.h>

#include <cube_vertices.h>

#include <glad/glad.h>

namespace vrl {

WorldFlat::WorldFlat(size_t extent_x, size_t extent_y, size_t extent_z)
    : IWorld(extent_x, extent_y, extent_z)
    , _VertexBuffer(VertexBuffer::VertexBufferBuilder("vert builder", g_cube_vertices)
                        .add_attribute(3)  // pos
                        .add_attribute(3)  // normal
                        .add_attribute(2)  // uv
                        .build()) {
}

// Voxel &WorldFlat::_get_voxel(size_t x, size_t y, size_t z) {
// }

void WorldFlat::_render() const {
	_VertexBuffer.use();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

}  // namespace vrl
