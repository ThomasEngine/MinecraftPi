#include "BlockRegistery.h"

BlockType g_BlockTypes[BlockTypeId::B_AMOUNT];

void InitializeBlockTypes()
{
    g_BlockTypes[B_AIR] = { "Air", {0,0,0,0,0,0}, false, true };
	g_BlockTypes[B_DIRT] = { "Dirt", {2,2,2,2,2,2}, true, false };
	g_BlockTypes[B_GRASS] = { "Grass", {3,3,2,0,3,3}, true, false };
	g_BlockTypes[B_STONE] = { "Stone", {2,2,2,2,2,2}, true, false };
	g_BlockTypes[B_COBBLESTONE] = { "Cobblestone", {17,17,17,17,17,17}, true, false };
	g_BlockTypes[B_OAK_LOG] = { "Oak Log", {3,3,3,3,3,3}, true, false };
	g_BlockTypes[B_OAK_PLANK] = { "Oak Plank", {3,3,3,3,3,3}, true, false };
	g_BlockTypes[B_BEDROCK] = { "Bedrock", {3,3,3,3,3,3}, true, false };
}

