#include "ItemRegistry.h"

ItemType g_ItemTypes[I_AMOUNT];

void InitializeItemTypes()
{
    g_ItemTypes[I_DIRT_BLOCK] = {
        "Dirt", 5, ItemCategory::Block, B_DIRT
    };
	g_ItemTypes[I_GRASS_BLOCK] = {
		"Grass", 6, ItemCategory::Block, B_GRASS
	};
    g_ItemTypes[I_STONE_BLOCK] = {
        "Stone", 10, ItemCategory::Block, B_STONE
    };
	g_ItemTypes[I_COBBLESTONE_BLOCK] = {
		"Cobblestone", 11, ItemCategory::Block, B_COBBLESTONE
	};
    g_ItemTypes[I_OAK_LOG_BLOCK] = {
        "Oak log", 8, ItemCategory::Block, B_OAK_LOG
    };
	g_ItemTypes[I_OAK_PLANK_BLOCK] = {
		"Oak plank", 9, ItemCategory::Block, B_OAK_PLANK
	};
	g_ItemTypes[I_OAK_LEAF_BLOCK] = {
		"Oak Leaf", 7, ItemCategory::Block, B_OAK_LEAF
	};
	g_ItemTypes[I_BEDROCK_BLOCK] = {
		"Bedrock", 12, ItemCategory::Block, B_BEDROCK
	};
	g_ItemTypes[I_SAND_BLOCK] = {
		"Sand", 18, ItemCategory::Block, B_SAND
	};
	g_ItemTypes[I_GRAVEL_BLOCK] = {
		"Gravel", 19, ItemCategory::Block, B_GRAVEL
	};
	g_ItemTypes[I_GRANITE_BLOCK] = {
		"Granite", 14, ItemCategory::Block, B_GRANITE
	};
	g_ItemTypes[I_DIORITE_BLOCK] = {
		"Diorite", 15, ItemCategory::Block, B_DIORITE
	};
	g_ItemTypes[I_GLOWSTONE_BLOCK] = {
		"Glowstone", 105, ItemCategory::Block, B_GLOWSTONE
	};

	// Non-block items
	g_ItemTypes[I_STICK] = {
		"Stick", 50, ItemCategory::Material, B_AIR
	};
	g_ItemTypes[I_WOODEN_SWORD] = {
		"Wooden Sword", 51, ItemCategory::Tool, B_AIR
	};
	g_ItemTypes[I_WOODEN_PICKAXE] = {
		"Wooden Pickaxe", 52, ItemCategory::Tool, B_AIR
	};
	g_ItemTypes[I_WOODEN_AXE] = {
		"Wooden Axe", 53, ItemCategory::Tool, B_AIR
	};
}
