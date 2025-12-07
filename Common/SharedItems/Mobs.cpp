#include "Mobs.h"
#include "Renderer.h"
#include <ext/matrix_transform.hpp>
#pragma once

Sheep::Sheep(SharedModelData* sharedData)
{
	this->sharedData = sharedData;
	this->instanceData.health = 10;
	this->instanceData.aiState = AiState::Wandering; 
}

void Sheep::render(Renderer& ren, Shader& sh, Texture& tex, glm::mat4 viewProj)
{
	glm::mat4 sheepModel = glm::translate(glm::mat4(1.0f), instanceData.position);
	// Body
	glm::mat4 bodyMat = sheepModel; // body transform from pose
	ren.drawMesh(sharedData->bodyMesh, sh, viewProj * bodyMat, tex);

	// Head
	glm::mat4 headMat = sheepModel; // head transform from pose
	ren.drawMesh(sharedData->headMesh, sh, viewProj * headMat, tex);

	// Legs
	for (int i = 0; i < 4; ++i) {
		glm::mat4 legMat = sheepModel; // leg transform from pose
		ren.drawMesh(sharedData->legMesh[i], sh, viewProj * legMat, tex);
	}
}

void Sheep::update(float deltaTime)
{
	UpdateBehavior(deltaTime);
	UpdateMovement(deltaTime);
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
}

void Sheep::UpdateWalkingAnimation(float deltaTime)
{
	// Legs move back and forth 
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
}

Villager::Villager(SharedModelData* sharedData)
{
	this->sharedData = sharedData;
	this->instanceData.health = 20;
}

//void Villager::render(Renderer& ren, Shader& sh, Texture& tex, glm::mat4 viewProj)
//{
//}
//
//void Villager::update(float deltaTime)
//{
//}
//
//Mob* Villager::clone()
//{
//	// shallow copy
//	Villager* v = new Villager(sharedData);
//	v->instanceData = this->instanceData;
//	return v;
//}
