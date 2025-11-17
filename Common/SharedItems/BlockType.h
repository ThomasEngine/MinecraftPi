#include <cstdint> 

enum BlockTypeId : uint8_t {
	B_AIR = 0,
	B_DIRT,
	B_GRASS,
	B_STONE,
	B_COBBLESTONE,
	B_OAK_LOG,
	B_OAK_PLANK,
	B_OAK_LEAF,
	B_BEDROCK,
	B_AMOUNT
};

struct BlockType {
	const char* name;
	uint8_t textureIndices[6];
	bool isSolid;
	bool isTransparent;
};

