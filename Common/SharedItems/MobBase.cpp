#include "MobBase.h"
#include "World.h"
#include "CollisionSystem.h"

void Mob::UpdateAnimation(float deltaTime)
{
	switch (instanceData.aiState) {
	case AiState::Chasing:
		UpdateChasingAnimation(deltaTime);
		break;
	case AiState::Wandering:
		UpdateWanderingAnimation(deltaTime);
		break;
	case AiState::Mating:
		UpdateMatingAnimation(deltaTime);
		break;
	default:
		break;
	}
}

void Mob::Move(float deltaTime)
{
	UpdateMovement(deltaTime);
	UpdateYMovement(deltaTime);
}

void Mob::moveTo(const glm::vec3& target)
{
	instanceData.moveTarget = target;
	instanceData.hasMovetarget = true;
	FindPath(target);
}

void Mob::Jump(float deltaTime)
{
	if (instanceData.onGround)
	{
		instanceData.velocity.y = JUMP_SPEED;
	}
}

void Mob::FindPath(const glm::vec3& target)
{
	instanceData.path.clear();
	instanceData.path.push_back(target);
	instanceData.pathIndex = 0;
}

void Mob::GetRandomWanderTarget()
{
    float offsetX = float(rand() % 11 - 5);
    float offsetZ = float(rand() % 11 - 5);
    glm::vec3 randomTarget = instanceData.position + glm::vec3(offsetX, 0, offsetZ);
    moveTo(randomTarget);
}

void Mob::UpdateMovement(float deltaTime)
{
	if (instanceData.hasMovetarget && !instanceData.path.empty() && instanceData.pathIndex < instanceData.path.size())
	{
		while (instanceData.pathIndex < instanceData.path.size()) {
			glm::vec3 moveTarget = instanceData.path[instanceData.pathIndex];
			glm::vec3 toTarget = moveTarget - instanceData.position;
			toTarget.y = 0; // ignore y difference for horizontal movement
			float dist = glm::length(toTarget);
			printf("Dist %.2f\n", dist);

			if (dist > 0.2f) {
				//instanceData.direction = instanceData.lastIndexChecked != instanceData.pathIndex ? toTarget / dist : instanceData.direction; // only recalculate direction if mob moved to a new path index
				instanceData.direction = toTarget / dist;
				glm::vec3 move = instanceData.direction * instanceData.speed * deltaTime;

				// X axis
				glm::vec3 tryX = instanceData.position + glm::vec3(move.x, 0, 0);
				bool blockedX = m_CS->CheckGridCollision(tryX, sharedData->hitbox);
				if (!blockedX)
					instanceData.position.x += move.x;

				// Z axis
				glm::vec3 tryZ = instanceData.position + glm::vec3(0, 0, move.z);
				bool blockedZ = m_CS->CheckGridCollision(tryZ, sharedData->hitbox);
				if (!blockedZ)
					instanceData.position.z += move.z;

				// If both are blocked try jumping
				if (blockedX || blockedZ) {
					glm::vec3 above = instanceData.position + glm::vec3(0, 1.0f, 0) + instanceData.direction;
					if (!m_CS->CheckGridCollision(above, sharedData->hitbox)) {
						Jump(deltaTime);
					}
					else
					{
						if (instanceData.aiState == AiState::Wandering)
							GetRandomWanderTarget();
					}
				}
	
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

void Mob::UpdateYMovement(float deltaTime)
{
	instanceData.velocity.y -= GRAVITY * deltaTime;
	if (instanceData.velocity.y > MAX_FALL_SPEED) instanceData.velocity.y = MAX_FALL_SPEED;

	glm::vec3 newPos = instanceData.position;
	float changeY = instanceData.velocity.y * deltaTime;
	newPos.y += changeY;

	if (!m_CS->CheckGridCollision(newPos, sharedData->hitbox))
	{
		instanceData.onGround = false;
		instanceData.position = newPos;
	}
	else
	{
		instanceData.onGround = true;
		instanceData.velocity.y = 0;
	}
}


void Mob::UpdateBehavior(float deltaTime)
{
	switch (instanceData.aiState) {
	case AiState::Chasing:
		UpdateChasingBehavior(deltaTime);
		break;
	case AiState::Wandering:
		UpdateWanderingBehavior(deltaTime);
		break;
	case AiState::Mating:
		UpdateMatingBehavior(deltaTime);
		break;
	default:
		break;
	}
}
