#pragma once

namespace vox {

enum class BlockType {
	AIR,
	GRASS,
	DIRT,
	STONE,
};

struct Block {
	BlockType _type;
};

}  // namespace vox
