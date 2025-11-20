#include "CollisionSystem.h"
#include "ChunkManager.h"


bool CollisionSystem::CheckGridCollision(glm::vec3 posToCheck)
{
    // Check if current position collides with a block
    // Check at player's corners
    return false;
    glm::ivec3 chunkPos = glm::floor(posToCheck / 16.0f);
    chunkPos.y = 0;
    for (int c = 0; c < 3; c++) // Player dimensions
    {
        for (int i = -1; i <= 1; i += 2) // -1 and 1
        {
            glm::vec3 testPos = posToCheck;
            const float offset = playerDimensions[c] * 0.5f * (i);
            testPos[c] += offset;
            glm::vec3 posInChunk = testPos - glm::vec3(chunkPos) * 16.0f;
            uint8_t blockId = chunkManager->GetBlockAtPosition(posInChunk, chunkPos);
            if (blockId != 0) return true;
        }
    }
    return false;
}

void CollisionSystem::SetTarget(ChunkManager* CM)
{
	chunkManager = CM;
}
