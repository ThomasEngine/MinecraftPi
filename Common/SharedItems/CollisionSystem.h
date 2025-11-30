#pragma once
#include <glm/glm.hpp>

class CollisionSystem
{
public:
	bool CheckGridCollision(glm::vec3 PosToCheck);

private:
	glm::vec3 playerDimensions = { 0.8f, 4.f, 0.6 }; 
};