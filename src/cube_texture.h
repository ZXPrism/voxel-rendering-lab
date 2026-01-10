#pragma once

#include <texture.h>

namespace vox {

class CubeTexture : public Texture {
public:
	explicit CubeTexture(const std::string &cubemap_path);
};

}  // namespace vox
