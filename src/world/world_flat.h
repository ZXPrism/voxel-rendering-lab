#pragma once

#include <interfaces/world.h>
#include <vertex_buffer.h>

namespace vrl {

class WorldFlat : public IWorld<WorldFlat> {
private:
	VertexBuffer _VertexBuffer;

public:
	WorldFlat(size_t extent_x, size_t extent_y, size_t extent_z);

	Voxel &_get_voxel(size_t x, size_t y, size_t z);
	void _render() const;
};

}  // namespace vrl
