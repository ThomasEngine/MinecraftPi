#include "ItemRegistry.h"

ItemType g_ItemTypes[I_AMOUNT];

void InitializeItemTypes()
{
    g_ItemTypes[I_DIRT_BLOCK] = {
		"Dirt", {2,0}, ItemCategory::Block, B_DIRT
    };
	g_ItemTypes[I_GRASS_BLOCK] = {
		"Grass", {3,0}, ItemCategory::Block, B_GRASS
	};
    g_ItemTypes[I_STONE_BLOCK] = {
        "Stone", {1,0}, ItemCategory::Block, B_STONE
    };
	g_ItemTypes[I_COBBLESTONE_BLOCK] = {
		"Cobblestone", {0,1}, ItemCategory::Block, B_COBBLESTONE
	};
    g_ItemTypes[I_OAK_LOG_BLOCK] = {
        "Oak log", {4,1}, ItemCategory::Block, B_OAK_LOG
    };
	g_ItemTypes[I_OAK_PLANK_BLOCK] = {
		"Oak plank", {4,0}, ItemCategory::Block, B_OAK_PLANK
	};
	g_ItemTypes[I_OAK_LEAF_BLOCK] = {
		"Oak Leaf", {5,3}, ItemCategory::Block, B_OAK_LEAF
	};
	g_ItemTypes[I_BEDROCK_BLOCK] = {
		"Bedrock", {1,1}, ItemCategory::Block, B_BEDROCK
	};
	g_ItemTypes[I_SAND_BLOCK] = {
		"Sand", {2,1}, ItemCategory::Block, B_SAND
	};
	g_ItemTypes[I_GRAVEL_BLOCK] = {
		"Gravel", {3,1}, ItemCategory::Block, B_GRAVEL
	};
	//g_ItemTypes[I_GRANITE_BLOCK] = {
	//	"Granite", 14, ItemCategory::Block, B_GRANITE
	//};
	//g_ItemTypes[I_DIORITE_BLOCK] = {
	//	"Diorite", 15, ItemCategory::Block, B_DIORITE
	//};
	//g_ItemTypes[I_GLOWSTONE_BLOCK] = {
	//	"Glowstone", 105, ItemCategory::Block, B_GLOWSTONE
	//};

	// Non-block items
	g_ItemTypes[I_STICK] = {
		"Stick", {23, 12}, ItemCategory::Material, B_AIR
	};
	g_ItemTypes[I_WOODEN_SWORD] = {
		"Wooden Sword", {20,13}, ItemCategory::Tool, B_AIR
	};
	g_ItemTypes[I_WOODEN_PICKAXE] = {
		"Wooden Pickaxe", {18,13}, ItemCategory::Tool, B_AIR
	};
	g_ItemTypes[I_WOODEN_AXE] = {
		"Wooden Axe", {16,13}, ItemCategory::Tool, B_AIR
	};
}
