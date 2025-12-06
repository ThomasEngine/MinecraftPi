#include "Mobs.h"
#pragma once

Sheep::Sheep(SharedModelData* sharedData)
{
	this->sharedData = sharedData;
	this->instanceData.health = 10;
}

void Sheep::render()
{
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

void Villager::render()
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
