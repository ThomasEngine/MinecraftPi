#pragma once
#include <glm/glm.hpp>

class World;
class CollisionSystem
{
public:
	bool CheckGridCollision(const glm::vec3& PosToCheck, const glm::vec3& Dimensions);
	bool CehckPlayerToBlock(const glm::vec3& PlayerPos, const glm::vec3& blockPos, const glm::vec3& playerDim);
	bool IsInWater(const glm::vec3& posToCheck, const glm::vec3& dimension);
	void SetBlockTarget(World& world) { m_TargetWorld = &world; }
private:
	World* m_TargetWorld = nullptr;
	bool m_WorldReady{ false };
};