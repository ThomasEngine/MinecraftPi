#include "MobBase.h"

void Mob::UpdateAnimation(float deltaTime)
{
	switch (instanceData.aiState) {
	case AiState::Chasing:
		break;
	case AiState::Wandering:
		break;
	case AiState::Mating:
		break;
	default:
		break;
	}
}

void Mob::moveTo(const glm::vec3& target)
{
	instanceData.moveTarget = target;
	instanceData.hasMovetarget = true;
	FindPath(target);
}

void Mob::FindPath(const glm::vec3& target)
{
	instanceData.path.clear();
	// Simple straight-line path for now
	instanceData.path.push_back(target);
	instanceData.pathIndex = 0;
}

void Mob::UpdateMovement(float deltaTime)
{
	if (instanceData.hasMovetarget && !instanceData.path.empty() && instanceData.pathIndex < instanceData.path.size())
	{
		while (instanceData.pathIndex < instanceData.path.size()) {
			glm::vec3 moveTarget = instanceData.path[instanceData.pathIndex];
			glm::vec3 toTarget = moveTarget - instanceData.position;
			float dist = glm::length(toTarget);

			if (dist > 0.1f) {
				instanceData.direction = instanceData.lastIndexChecked != instanceData.pathIndex ? glm::normalize(toTarget) : instanceData.direction; // only recalculate direction if mob moved to a new path index
				instanceData.position += instanceData.direction * instanceData.speed * deltaTime;
				instanceData.lastIndexChecked = instanceData.pathIndex;
				break; 
			}
			else {
				instanceData.pathIndex++;
			}
		}

		if (instanceData.pathIndex >= instanceData.path.size()) {
			instanceData.hasMovetarget = false;
			instanceData.pathIndex = 0;
			instanceData.path.clear();
		}
	}
}


void Mob::UpdateBehavior(float deltaTime)
{
	switch (instanceData.aiState) {
	case AiState::Chasing:
		break;
	case AiState::Wandering:
		break;
	case AiState::Mating:
		break;
	default:
		break;
	}
}
