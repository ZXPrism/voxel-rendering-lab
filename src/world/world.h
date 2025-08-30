#pragma once

#include <gfx-utils-core/shader_program.h>
#include <world/voxel.h>

#include <glm/gtc/noise.hpp>

#include <random>

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

	void init() {
		std::random_device rd;
		std::mt19937_64 engine(rd());
		std::normal_distribution<double> dist(0.5, 1);

		for (size_t x = 0; x < _ExtentX; x++) {
			for (size_t z = 0; z < _ExtentZ; z++) {
				size_t height = static_cast<size_t>(dist(engine) * static_cast<double>(_ExtentY));
				for (size_t y = 0; y < _ExtentY; y++) {
					// float noise = glm::perlin(glm::vec3(x, y, z) * 0.04f);
					if (y > height) {
						get_voxel(x, y, z).type = -1;
					} else {
						get_voxel(x, y, z).type = 0;
					}
				}
			}
		}
	}

	Voxel &get_voxel(size_t x, size_t y, size_t z) {
		return static_cast<Derived *>(this)->_get_voxel(x, y, z);
	}

	const Voxel &get_voxel(size_t x, size_t y, size_t z) const {
		return static_cast<const Derived *>(this)->_get_voxel(x, y, z);
	}

	void render(gfxutils::ShaderProgram &shader_program) const {
		static_cast<const Derived *>(this)->_render(shader_program);
	}

protected:
	IWorld() = default;
	~IWorld() = default;
};

}  // namespace vrl
