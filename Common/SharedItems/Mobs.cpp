#include "Mobs.h"
#include "Renderer.h"
#include <ext/matrix_transform.hpp>
#include <cmath>
#pragma once

Sheep::Sheep(SharedModelData* sharedData)
	: pose()
{
	this->sharedData = sharedData;
	this->instanceData.health = 10;
	this->instanceData.aiState = AiState::Wandering; 
	this->instanceData.speed = 2.0f;
	this->instanceData.lastIndexChecked = 1;
	this->instanceData.timer = 0.f;
	this->pose.bodyRotation = 0.f;
}

void Sheep::render(Renderer& ren, Shader& sh, Texture& tex, glm::mat4 viewProj)
{
	glm::mat4 sheepModel = glm::translate(glm::mat4(1.0f), instanceData.position);
	
	glm::vec3 forward = instanceData.direction;
	glm::vec3 pos = instanceData.position;
	glm::vec3 up = glm::vec3(0, 1, 0);

	float targetAngle = std::atan2(forward.x, forward.z);
	float angleDiff = targetAngle - pose.bodyRotation;
	if (fabs(angleDiff) > 0.05f) // ~3 degrees
	{
		// Smoothly rotate towards targetAngle
		float turnSpeed = 0.1; // adjust as needed
		if (angleDiff > 0)
			pose.bodyRotation += std::min(angleDiff, turnSpeed);
		else
			pose.bodyRotation += std::max(angleDiff, -turnSpeed);
	}
	else
	{
		pose.bodyRotation = targetAngle;
	}

	// current angle
	sheepModel = glm::rotate(sheepModel, pose.bodyRotation, up);
	//sheepModel = glm::rotate(sheepModel, std::atan2(forward.x, forward.z), up);

	// Body
	glm::mat4 bodyMat = sheepModel; 
	ren.drawMesh(sharedData->bodyMesh, sh, viewProj * bodyMat, tex);

	// Head
	glm::mat4 headMat = sheepModel; 
	headMat = glm::rotate(headMat, glm::radians(pose.headRotation), glm::vec3(0, 1, 0));
	ren.drawMesh(sharedData->headMesh, sh, viewProj * headMat, tex);

	for (int i = 0; i < 4; ++i) {
		glm::vec3 legTopPos = sharedData->legTopPosition[i];
		glm::mat4 legMat = sheepModel;
		legMat = glm::translate(legMat, legTopPos); 
		legMat = glm::rotate(legMat, glm::radians(pose.legRotation[i]), glm::vec3(1, 0, 0)); 
		legMat = glm::translate(legMat, -legTopPos);
		ren.drawMesh(sharedData->legMesh[i], sh, viewProj * legMat, tex);
	}
}

void Sheep::update(float deltaTime)
{
	UpdateBehavior(deltaTime);
	Move(deltaTime);
	UpdateAnimation(deltaTime);
}

Mob* Sheep::clone()
{
	// shallow copy
	Sheep* v = new Sheep(sharedData);
	v->instanceData = this->instanceData; 
	return v;
}

void Sheep::UpdateChasingBehavior(float deltaTime)
{
	// Looking at player and walking towards them
}

void Sheep::UpdateMatingBehavior(float deltaTime)
{
	// Looking at connected sheep and walk to a meeting point
}

void Sheep::UpdateWanderingBehavior(float deltaTime)
{
	// Randomly walk around and stop occasionally for grass
	if (!instanceData.hasMovetarget)
	{
		instanceData.timer += deltaTime;
		if (instanceData.timer >= instanceData.waitTime) 
		{
			instanceData.timer = 0.0f;
			GetRandomWanderTarget();
			// Walk
			instanceData.walkState = WalkingState::Walking;
			instanceData.waitTime = 2 + rand() % 7;
		}
		else
		{
			// Idle
			instanceData.walkState = WalkingState::Idle;
		}
	}
}

void Sheep::UpdateWalkingAnimation(float deltaTime)
{
	// Legs move back and forth
	// Advance the walk cycle timer
	pose.walkTime += deltaTime;

	float amplitude = 30.0f; 
	float speed = 5.0f;      
	float headSpeed = 0.1f;
	// Animate legs with phase offsets for natural movement
	pose.legRotation[Legs::FL] = amplitude * std::sin(speed * pose.walkTime);
	pose.legRotation[Legs::BR] = amplitude * std::sin(speed * pose.walkTime);
	pose.legRotation[Legs::FR] = -amplitude * std::sin(speed * pose.walkTime);
	pose.legRotation[Legs::BL] = -amplitude * std::sin(speed * pose.walkTime);

	pose.headRotation = 15.0f * std::sin(headSpeed * pose.walkTime);

	printf("Walking\r");
}

void Sheep::UpdateIdleAnimation(float deltaTime)
{
	printf("Idle\n");
	// keep rotating the leg until they are straight

	pose.walkTime += deltaTime;

	float amplitude = 30.0f;
	float speed = 5.0f;


	// Animate legs until they are straight (rotation approaches 0)
	pose.walkTime += deltaTime;

	float relaxSpeed = 120.0f * deltaTime; // degrees per second
	for (int i = 0; i < 4; ++i) {
		if (std::fabs(pose.legRotation[i]) > 1.0f) {
			// Move leg rotation towards zero
			if (pose.legRotation[i] > 0.0f)
				pose.legRotation[i] = std::max(0.0f, pose.legRotation[i] - relaxSpeed);
			else
				pose.legRotation[i] = std::min(0.0f, pose.legRotation[i] + relaxSpeed);
		}
		else {
			pose.legRotation[i] = 0.0f;
		}
	}

	// Randomly look around
	float headSpeed = 1.0f;
	pose.headRotation = 15.0f * std::sin(headSpeed * pose.walkTime);
}

void Sheep::UpdateRunningAnimation(float deltaTime)
{
}

void Sheep::UpdateWanderingAnimation(float deltaTime)
{
	// Moves legs
	switch (instanceData.walkState)
	{
	case WalkingState::Idle:
		UpdateIdleAnimation(deltaTime);
		break;
	case WalkingState::Walking:
		UpdateWalkingAnimation(deltaTime);
		break;
	case WalkingState::Running:
		UpdateWalkingAnimation(deltaTime);
		break;
	}
}

void Sheep::UpdateChasingAnimation(float deltaTime)
{
	// Legs move back and forth looking towards player
}

void Sheep::UpdateMatingAnimation(float deltaTime)
{
	// Standing still looking at connected sheep
}

void Sheep::CheckStateTransition()
{
	// Transition between Wandering, Chasing, Mating based on conditions
}
