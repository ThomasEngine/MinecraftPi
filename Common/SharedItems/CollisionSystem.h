#pragma once
#include <glm/glm.hpp>

class ChunkManager;
class CollisionSystem
{
public:
	bool CheckGridCollision(glm::vec3 PosToCheck);
	void SetTarget(ChunkManager* CM);

private:
	glm::vec3 playerDimensions = { 0.8f, 4.f, 0.6 }; // Width, Height, Depth
	ChunkManager* chunkManager = nullptr;
};