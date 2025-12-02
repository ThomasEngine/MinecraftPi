#pragma once
#include <glm/glm.hpp>

class World;
class CollisionSystem
{
public:
	bool CheckGridCollision(glm::vec3 PosToCheck);
	bool CehckPlayerToBlock(const glm::vec3& PlayerPos, const glm::vec3& blockPos);
	void SetPlayerDimensions(const glm::vec3& dimensions) { m_PlayerDimensions = dimensions; }
	void SetBlockTarget(World& world) { m_TargetWorld = &world; }
private:
	glm::vec3 m_PlayerDimensions = { 0.8f, 1.9f, 0.6 }; 
	World* m_TargetWorld = nullptr;
};