#include "BlockType.h"
#include "Rendering/include/Renderer.h"

extern BlockType g_BlockTypes[BlockTypeId::B_AMOUNT];

//struct BlockInstanceData {
//    glm::vec3 position;
//    uint8_t atlasIndex;
//};

// Voxel structure
struct Voxel
{
    uint8_t blockID;
    uint8_t lightLevel;
};

void InitializeBlockTypes();