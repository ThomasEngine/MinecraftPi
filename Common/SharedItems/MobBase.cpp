#include "MobBase.h"
#include "World.h"
#include "CollisionSystem.h"
#include <cmath>
#include <algorithm> 


// Movement seperator
void Mob::Move(float deltaTime)
{
	UpdateMovement(deltaTime);
	UpdateYMovement(deltaTime);
}

// Move to target position
void Mob::moveTo(const glm::vec3& target)
{
	instanceData.moveTarget = target;
	instanceData.hasMovetarget = true;
	FindPath(target);
}

// Jump action
void Mob::Jump(float deltaTime)
{
	if (instanceData.onGround)
	{
		instanceData.velocity.y = JUMP_SPEED;
	}
}

// For now only similair to moveTo, but want to expand to A* pathfinding later
void Mob::FindPath(const glm::vec3& target)
{
	instanceData.path.clear();
	instanceData.path.push_back(target);
	instanceData.pathIndex = 0;
}

// Get random target nearby for wandering
void Mob::GetRandomWanderTarget()
{
    float offsetX = float(rand() % 15 - 7);
    float offsetZ = float(rand() % 15 - 7);
    glm::vec3 randomTarget = instanceData.position + glm::vec3(offsetX, 0, offsetZ);
    moveTo(randomTarget);
}

// Update horizontal movement towards path target
void Mob::UpdateMovement(float deltaTime)
{
	if (instanceData.hasMovetarget && !instanceData.path.empty() && instanceData.pathIndex < instanceData.path.size())
	{
		while (instanceData.pathIndex < instanceData.path.size()) {
			glm::vec3 moveTarget = instanceData.path[instanceData.pathIndex];
			glm::vec3 toTarget = moveTarget - instanceData.position;
			toTarget.y = 0; // ignore y difference for horizontal movement
			float dist = glm::length(toTarget);

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

// Update vertical movement with gravity and collision
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

// Behavior and animation updaters (state machine)
void Mob::UpdateBehavior(float deltaTime, const glm::vec3& playerPos)
{
	pose->updateHeadMovement(deltaTime, playerPos);
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

// Animation updaters (state machine)
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

// Wandering animation (state machine)
void Mob::UpdateWanderingAnimation(float deltaTime)
{
	switch (instanceData.walkState) {
	case WalkingState::Walking:
		pose->walkingAnimation(deltaTime);
		break;
	case WalkingState::Idle:
		pose->idleAnimation(deltaTime);
		break;
	case WalkingState::Running:
		pose->walkingAnimation(deltaTime * 2);
		break;

	}
}


void Mob::UpdateChasingAnimation(float deltaTime)
{
}

void Mob::UpdateMatingAnimation(float deltaTime)
{
}
// /////////////////////////
// /////////////////////////
// /////////////////////////
// Animation implementations
// /////////////////////////
// /////////////////////////
// //All fourse animation //

void AllFourAnimations::walkingAnimation(float deltaTime)
{
	// Update walk time
	walkTime += deltaTime;
	
	// Parameters for leg movement
	const float amplitude = 30.0f;
	const float speed = 5.0f;
	const float headSpeed = 0.1f;

	// Animate legs with phase offsets for natural movement
	legRotation[Legs::FrontLeft] = amplitude * std::sin(speed * walkTime);
	legRotation[Legs::BackRight] = amplitude * std::sin(speed * walkTime);
	legRotation[Legs::FrontRight] = -amplitude * std::sin(speed * walkTime);
	legRotation[Legs::BackLeft] = -amplitude * std::sin(speed * walkTime);
}

void AllFourAnimations::idleAnimation(float deltaTime)
{
	// Update walk time
	walkTime += deltaTime;

	// Parameters for leg relaxation
	const float amplitude = 30.0f;
	const float speed = 5.0f;

	// Relax legs back to neutral position by reducing rotation each frame
	float relaxSpeed = 120.0f * deltaTime; 
	for (int i = 0; i < 4; ++i) {
		if (std::fabs(legRotation[i]) > 1.0f) { // leg rotation away from neutral
			if (legRotation[i] > 0.0f) 
				legRotation[i] = std::max(0.0f, legRotation[i] - relaxSpeed);
			else
				legRotation[i] = std::min(0.0f, legRotation[i] + relaxSpeed);
		}
		else { // close enough to neutral
			legRotation[i] = 0.0f;
		}
	}


}

void PoseBase::updateHeadMovement(float targetAngle, const glm::vec3& playerPos)
{
	// Simple head looking around animation
	float headSpeed = 1.0f;
	headRotation = 15.0f * std::sin(headSpeed * walkTime);
	// Want to ad more later:
	// Sometimes look at player
	// Look at other mob or other things idk
}

// Smoothly rotate body towards target angle same for all mobs
void PoseBase::updateBodyRotation(float targetAngle)
{
	float angleDiff = targetAngle - bodyRotation;
	if (fabs(angleDiff) > 0.05f)
	{
		// Smoothly rotate towards targetAngle
		float turnSpeed = 0.1;
		if (angleDiff > 0)
			bodyRotation += std::min(angleDiff, turnSpeed);
		else
			bodyRotation += std::max(angleDiff, -turnSpeed);
	}
	else
	{
		bodyRotation = targetAngle;
	}
}
