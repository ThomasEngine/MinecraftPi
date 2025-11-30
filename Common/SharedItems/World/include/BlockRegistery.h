#pragma once

#include "World/include/BlockType.h"
#include "Rendering/include/Renderer.h"

extern BlockType g_BlockTypes[BlockTypeId::B_AMOUNT];

// Voxel structure
struct Voxel
{
    uint8_t blockID;
    uint8_t lightLevel;
};

void InitializeBlockTypes();