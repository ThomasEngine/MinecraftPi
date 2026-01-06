#pragma once
#include <cstdint> 
#include <string>

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
	B_SAND,
	B_GRAVEL,
	B_WATER,
	B_GRANITE,
	B_DIORITE,
	B_GLOWSTONE,
	B_CRAFTING_TABLE,
	B_AMOUNT
};

struct AtlasPos {
	uint8_t x;
	uint8_t y;

	void operator =(const AtlasPos& other) {
		x = other.x;
		y = other.y;
	}
};

struct Interactable {
	// Data necessary for items
	
};

struct BlockType {
	const char* name;
	AtlasPos textureIndices[6];
	uint8_t itemID = 0; 
	bool isSolid;
	bool isTransparent;
	bool mobSpawningAllowed = true;
	uint8_t lightStrength = 0;
	Interactable* interaction = nullptr;

	BlockType() = default;

	BlockType(const char* name, const AtlasPos list[6], bool solid, bool transparent, uint8_t light = 0)
		: name(name), isSolid(solid), isTransparent(transparent), lightStrength(light)
	{
		for (int i = 0; i < 6; i++)
			textureIndices[i] = list[i];
	}
};

