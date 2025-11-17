#include "BlockType.h"
#include "Renderer.h"

extern BlockType g_BlockTypes[BlockTypeId::B_AMOUNT];

struct BlockInstanceData {
    glm::vec3 position;
    uint8_t atlasIndex;
};

void InitializeBlockTypes();