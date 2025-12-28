#pragma once
#include <cstdint>
#include "BlockType.h"

enum class ItemCategory : uint8_t {
	Block,
	Tool,
	Food,
	Material,
	Count, // Can add more when I want more itemtypes
};

enum ItemTypeId : uint8_t {
    I_NONE = 0,
    I_DIRT_BLOCK,      // Block items
    I_GRASS_BLOCK,
    I_STONE_BLOCK,
    I_COBBLESTONE_BLOCK,
    I_OAK_LOG_BLOCK,
    I_OAK_PLANK_BLOCK,
    I_OAK_LEAF_BLOCK,
    I_BEDROCK_BLOCK,
    I_SAND_BLOCK,
    I_GRAVEL_BLOCK,
    I_GRANITE_BLOCK,
    I_DIORITE_BLOCK,
    I_GLOWSTONE_BLOCK,
    // Non-block items
    I_STICK,
    I_WOODEN_SWORD,
    I_WOODEN_PICKAXE,
    I_WOODEN_AXE,
    I_AMOUNT
};

struct AtlasPos {
	uint8_t x;
	uint8_t y;
};

struct ItemType {
	std::string name;
	AtlasPos inventoryTextureIndex;
	ItemCategory category;

	BlockTypeId blockType = B_AIR; // B_AIR for no block

	// Additional properties for tools, food, etc.
};
