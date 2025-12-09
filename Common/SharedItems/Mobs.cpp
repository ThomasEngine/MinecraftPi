#pragma once

#include "Mobs.h"
#include "Renderer.h"
#include <ext/matrix_transform.hpp>
#include <cmath>

FourlegMob::FourlegMob(SharedModelData* sharedData)
{
	pose = new AllFourAnimations();
	this->sharedData = sharedData;
	this->instanceData.health = 10;
	this->instanceData.aiState = AiState::Wandering; 
	this->instanceData.speed = 2.0f;
	this->instanceData.lastIndexChecked = 1;
	this->instanceData.timer = 0.f;
	this->pose->bodyRotation = 0.f;
}

void FourlegMob::render(Renderer& ren, Shader& sh, Texture& tex, glm::mat4 viewProj)
{
	glm::mat4 sheepModel = glm::translate(glm::mat4(1.0f), instanceData.position);
	
	glm::vec3 forward = instanceData.direction;
	glm::vec3 up = glm::vec3(0, 1, 0);

	float targetAngle = std::atan2(forward.x, forward.z);
	pose->updateBodyRotation(targetAngle);

	sheepModel = glm::rotate(sheepModel, pose->bodyRotation, up);

	// Body
	glm::mat4 bodyMat = sheepModel; 
	ren.drawMesh(sharedData->bodyMesh, sh, viewProj * bodyMat, tex);

	// Head
	glm::mat4 headMat = sheepModel; 
	headMat = glm::rotate(headMat, glm::radians(pose->headRotation), glm::vec3(0, 1, 0));
	ren.drawMesh(sharedData->headMesh, sh, viewProj * headMat, tex);

	for (int i = 0; i < 4; ++i) {
		glm::vec3 legTopPos = sharedData->legTopPosition[i];
		glm::mat4 legMat = sheepModel;
		legMat = glm::translate(legMat, legTopPos); 
		legMat = glm::rotate(legMat, glm::radians(pose->legRotation[i]), glm::vec3(1, 0, 0)); 
		legMat = glm::translate(legMat, -legTopPos);
		ren.drawMesh(sharedData->legMesh[i], sh, viewProj * legMat, tex);
	}
}

void FourlegMob::update(float deltaTime, const glm::vec3& playerPos)
{
	UpdateBehavior(deltaTime, playerPos);
	Move(deltaTime);
	UpdateAnimation(deltaTime);
}

Mob* FourlegMob::clone()
{
	// shallow copy
	FourlegMob* v = new FourlegMob(sharedData);
	v->instanceData = this->instanceData; 
	return v;
}

void FourlegMob::UpdateChasingBehavior(float deltaTime)
{
	// Looking at player and walking towards them
}

void FourlegMob::UpdateMatingBehavior(float deltaTime)
{
	// Looking at connected sheep and walk to a meeting point
}

void FourlegMob::UpdateWanderingBehavior(float deltaTime)
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

void FourlegMob::CheckStateTransition()
{
	// Transition between Wandering, Chasing, Mating based on conditions
}
