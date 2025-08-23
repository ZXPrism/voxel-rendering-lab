#pragma once

#include <world/voxel.h>

namespace vrl {

template<typename Derived>
class IWorld {
protected:
	size_t _ExtentX;
	size_t _ExtentY;
	size_t _ExtentZ;

public:
	IWorld(size_t extent_x, size_t extent_y, size_t extent_z)
	    : _ExtentX(extent_x)
	    , _ExtentY(extent_y)
	    , _ExtentZ(extent_z) {
	}

	Voxel &get_voxel(size_t x, size_t y, size_t z) {
		return static_cast<const Derived *>(this)->_get_voxel(x, y, z);
	}

	void render() const {
		static_cast<const Derived *>(this)->_render();
	}

protected:
	IWorld() = default;
	~IWorld() = default;
};

}  // namespace vrl
