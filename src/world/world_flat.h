#pragma once

#include <interfaces/world.h>
#include <vertex_buffer.h>
#include <world/voxel.h>

namespace vrl {

class WorldFlat : public IWorld<WorldFlat> {
private:
	VertexBuffer _VertexBuffer;
	std::vector<Voxel> _WorldData;

public:
	WorldFlat(size_t extent_x, size_t extent_y, size_t extent_z);

	Voxel &_get_voxel(size_t x, size_t y, size_t z);
	const Voxel &_get_voxel(size_t x, size_t y, size_t z) const;

	void _render(ShaderProgram &shader_program) const;
};

}  // namespace vrl
