#include "Mobs.h"
#include "Renderer.h"
#include <ext/matrix_transform.hpp>
#pragma once

Sheep::Sheep(SharedModelData* sharedData)
{
	this->sharedData = sharedData;
	this->instanceData.health = 10;
}

void Sheep::render(Renderer& ren, Shader& sh, Texture& tex, glm::mat4 viewProj)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), instanceData.position);
	glm::mat4 mvp = viewProj * model;
	ren.drawMesh(sharedData->mesh, sh, mvp, tex);
}

void Sheep::update(float deltaTime)
{
}

Mob* Sheep::clone()
{
	// shallow copy
	Sheep* v = new Sheep(sharedData);
	v->instanceData = this->instanceData; 
	return v;
}

Villager::Villager(SharedModelData* sharedData)
{
	this->sharedData = sharedData;
	this->instanceData.health = 20;
}

void Villager::render(Renderer& ren, Shader& sh, Texture& tex, glm::mat4 viewProj)
{
}

void Villager::update(float deltaTime)
{
}

Mob* Villager::clone()
{
	// shallow copy
	Villager* v = new Villager(sharedData);
	v->instanceData = this->instanceData;
	return v;
}
