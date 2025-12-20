#include "BlockRegistery.h"

BlockType g_BlockTypes[BlockTypeId::B_AMOUNT];

// Old initializer using initializer lists
//void InitializeBlockTypes()
//{
//    g_BlockTypes[B_AIR] = BlockType("Air", { 0, 0, 0, 0, 0, 0 }, false, true);
//    g_BlockTypes[B_DIRT] = BlockType("Dirt", { 2, 2, 2, 2, 2, 2 }, true, false);
//    g_BlockTypes[B_GRASS] = BlockType("Grass", (uint8_t[6]){ 3, 3, 2, 0, 3, 3 }, true, false);
//    g_BlockTypes[B_STONE] = BlockType("Stone", (uint8_t[6]){ 1, 1, 1, 1, 1, 1 }, true, false);
//    g_BlockTypes[B_SAND] = BlockType("Sand", (uint8_t[6]){ 18, 18, 18, 18, 18, 18 }, true, false);
//    g_BlockTypes[B_GRAVEL] = BlockType("Gravel", (uint8_t[6]){ 19, 19, 19, 19, 19, 19 }, true, false);
//    g_BlockTypes[B_COBBLESTONE] = BlockType("Cobblestone", (uint8_t[6]){ 17, 17, 17, 17, 17, 17 }, true, false);
//    g_BlockTypes[B_OAK_LOG] = BlockType("Oak Log", (uint8_t[6]){ 3, 3, 3, 3, 3, 3 }, true, false);
//    g_BlockTypes[B_OAK_PLANK] = BlockType("Oak Plank", (uint8_t[6]){ 3, 3, 3, 3, 3, 3 }, true, false);
//    g_BlockTypes[B_BEDROCK] = BlockType("Bedrock", (uint8_t[6]){ 3, 3, 3, 3, 3, 3 }, true, false);
//    g_BlockTypes[B_WATER] = BlockType("Water", (uint8_t[6]){ 205, 205, 205, 205, 205, 205 }, false, true);
//    g_BlockTypes[B_GLOWSTONE] = BlockType("Glowstone", (uint8_t[6]){ 105, 105, 105, 105, 105, 105 }, true, false, 15);
//}

// Did not work for the pi compiler, so had to do it another way
void InitializeBlockTypes()
{
    // Air
    g_BlockTypes[B_AIR] = BlockType();
	g_BlockTypes[B_AIR].name = "Air";
	g_BlockTypes[B_AIR].isSolid = false;
	g_BlockTypes[B_AIR].isTransparent = true;
    for (int i = 0; i < 6; i++)
    {
        g_BlockTypes[B_AIR].textureIndices[i] = 0;
    }

    // Dirt
    g_BlockTypes[B_DIRT] = BlockType();
    g_BlockTypes[B_DIRT].name = "Dirt";
    g_BlockTypes[B_DIRT].isSolid = true;
    g_BlockTypes[B_DIRT].isTransparent = false;
    for (int i = 0; i < 6; i++)
    {
        g_BlockTypes[B_DIRT].textureIndices[i] = 2;
    }

	// Grass
	g_BlockTypes[B_GRASS] = BlockType();
	g_BlockTypes[B_GRASS].name = "Grass";
	g_BlockTypes[B_GRASS].isSolid = true;
	g_BlockTypes[B_GRASS].isTransparent = false;
	g_BlockTypes[B_GRASS].textureIndices[0] = 3; // Top
	g_BlockTypes[B_GRASS].textureIndices[1] = 3; // Bottom
	g_BlockTypes[B_GRASS].textureIndices[2] = 2; // Front
	g_BlockTypes[B_GRASS].textureIndices[3] = 0; // Back
	g_BlockTypes[B_GRASS].textureIndices[4] = 3; // Left
	g_BlockTypes[B_GRASS].textureIndices[5] = 3; // Right

	// Stone
	g_BlockTypes[B_STONE] = BlockType();
	g_BlockTypes[B_STONE].name = "Stone";
	g_BlockTypes[B_STONE].isSolid = true;
	g_BlockTypes[B_STONE].isTransparent = false;
	for (int i = 0; i < 6; i++)
	{
		g_BlockTypes[B_STONE].textureIndices[i] = 1;
	}

	// Sand
	g_BlockTypes[B_SAND] = BlockType();
	g_BlockTypes[B_SAND].name = "Sand";
	g_BlockTypes[B_SAND].isSolid = true;
	g_BlockTypes[B_SAND].isTransparent = false;
	for (int i = 0; i < 6; i++)
	{
		g_BlockTypes[B_SAND].textureIndices[i] = 18;
	}

	// Gravel
	g_BlockTypes[B_GRAVEL] = BlockType();
	g_BlockTypes[B_GRAVEL].name = "Gravel";
	g_BlockTypes[B_GRAVEL].isSolid = true;
	g_BlockTypes[B_GRAVEL].isTransparent = false;
	for (int i = 0; i < 6; i++)
	{
		g_BlockTypes[B_GRAVEL].textureIndices[i] = 19;
	}

	// Cobblestone
	g_BlockTypes[B_COBBLESTONE] = BlockType();
	g_BlockTypes[B_COBBLESTONE].name = "Cobblestone";
	g_BlockTypes[B_COBBLESTONE].isSolid = true;
	g_BlockTypes[B_COBBLESTONE].isTransparent = false;
	for (int i = 0; i < 6; i++)
	{
		g_BlockTypes[B_COBBLESTONE].textureIndices[i] = 17;
	}

	// Oak Log
	g_BlockTypes[B_OAK_LOG] = BlockType();
	g_BlockTypes[B_OAK_LOG].name = "Oak Log";
	g_BlockTypes[B_OAK_LOG].isSolid = true;
	g_BlockTypes[B_OAK_LOG].isTransparent = false;
	g_BlockTypes[B_OAK_LOG].textureIndices[0] = 20; // Top
	g_BlockTypes[B_OAK_LOG].textureIndices[1] = 20; // Bottom
	g_BlockTypes[B_OAK_LOG].textureIndices[2] = 21; // Front
	g_BlockTypes[B_OAK_LOG].textureIndices[3] = 21; // Back
	g_BlockTypes[B_OAK_LOG].textureIndices[4] = 20; // Left
	g_BlockTypes[B_OAK_LOG].textureIndices[5] = 20; // Right

	// Oak Plank
	g_BlockTypes[B_OAK_PLANK] = BlockType();
	g_BlockTypes[B_OAK_PLANK].name = "Oak Plank";
	g_BlockTypes[B_OAK_PLANK].isSolid = true;
	g_BlockTypes[B_OAK_PLANK].isTransparent = false;
	for (int i = 0; i < 6; i++)
	{
		g_BlockTypes[B_OAK_PLANK].textureIndices[i] = 4;
	}

	// Bedrock
	g_BlockTypes[B_BEDROCK] = BlockType();
	g_BlockTypes[B_BEDROCK].name = "Bedrock";
	g_BlockTypes[B_BEDROCK].isSolid = true;
	g_BlockTypes[B_BEDROCK].isTransparent = false;
	for (int i = 0; i < 6; i++)
	{
		g_BlockTypes[B_BEDROCK].textureIndices[i] = 16;
	}

	// Water
	g_BlockTypes[B_WATER] = BlockType();
	g_BlockTypes[B_WATER].name = "Water";
	g_BlockTypes[B_WATER].isSolid = false;
	g_BlockTypes[B_WATER].isTransparent = true;
	for (int i = 0; i < 6; i++)
	{
		g_BlockTypes[B_WATER].textureIndices[i] = 205;
	}

	// Glowstone
	g_BlockTypes[B_GLOWSTONE] = BlockType();
	g_BlockTypes[B_GLOWSTONE].name = "Glowstone";
	g_BlockTypes[B_GLOWSTONE].isSolid = true;
	g_BlockTypes[B_GLOWSTONE].isTransparent = false;
	for (int i = 0; i < 6; i++)
	{
		g_BlockTypes[B_GLOWSTONE].textureIndices[i] = 105;
	}
	g_BlockTypes[B_GLOWSTONE].lightStrength = 15;

	// Leaves
	g_BlockTypes[B_OAK_LEAF] = BlockType();
	g_BlockTypes[B_OAK_LEAF].name = "Oak Leaf";
	g_BlockTypes[B_OAK_LEAF].isSolid = true;
	g_BlockTypes[B_OAK_LEAF].isTransparent = false;
	for (int i = 0; i < 6; i++)
	{
		//g_BlockTypes[B_OAK_LEAF].textureIndices[i] = 15 * 3 + 5; diamond ore
		g_BlockTypes[B_OAK_LEAF].textureIndices[i] = 15 * 3 + 8;
	}	
}