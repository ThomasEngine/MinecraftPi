#include "CollisionSystem.h"
#include "World.h"	

bool CollisionSystem::CheckGridCollision(glm::vec3 posToCheck)
{
    if (!m_TargetWorld) return false;

    glm::vec3 halfDims = m_PlayerDimensions * 0.5f;
    glm::vec3 minCorner = posToCheck - halfDims;
    glm::vec3 maxCorner = posToCheck + halfDims;

    for (int x = int(std::floor(minCorner.x)); x <= int(std::floor(maxCorner.x)); ++x) {
        for (int y = int(std::floor(minCorner.y)); y <= int(std::floor(maxCorner.y)); ++y) {
            for (int z = int(std::floor(minCorner.z)); z <= int(std::floor(maxCorner.z)); ++z) {
                glm::vec3 blockPos(x, y, z);
                uint8_t block = m_TargetWorld->GetBlockAtPosition(blockPos);
                if (block != 0) {
                    return true; // Collision detected
                }
            }
        }
    }
    return false; // No collision
}

bool CollisionSystem::CehckPlayerToBlock(const glm::vec3& PlayerPos, const glm::vec3& blockPos)
{
	glm::vec3 halfDims = m_PlayerDimensions * 0.5f;
	glm::vec3 minCorner = PlayerPos - halfDims;
	glm::vec3 maxCorner = PlayerPos + halfDims;
	if (minCorner.x <= blockPos.x + 1 && maxCorner.x >= blockPos.x &&
		minCorner.y <= blockPos.y + 1 && maxCorner.y >= blockPos.y &&
		minCorner.z <= blockPos.z + 1 && maxCorner.z >= blockPos.z)
	{
		return true;
	}
    return false;
}


