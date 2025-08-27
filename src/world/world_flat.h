#pragma once

#include <world/voxel.h>
#include <world/world.h>

#include <gfx-utils-core/vertex_buffer.h>

namespace vrl {

class WorldFlat : public IWorld<WorldFlat> {
private:
	gfxutils::VertexBuffer _VertexBuffer;
	std::vector<Voxel> _WorldData;

public:
	WorldFlat(size_t extent_x, size_t extent_y, size_t extent_z);

	Voxel &_get_voxel(size_t x, size_t y, size_t z);
	const Voxel &_get_voxel(size_t x, size_t y, size_t z) const;

	void _render(gfxutils::ShaderProgram &shader_program) const;
};

}  // namespace vrl
