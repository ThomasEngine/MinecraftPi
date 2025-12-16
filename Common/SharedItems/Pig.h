#pragma once
#include "Mobs.h"

class Pig : public FourlegMob
{
public:
	Pig(SharedModelData* sharedData) : FourlegMob(sharedData) {
		this->instanceData.health = 10;
		this->instanceData.speed = 2.0f;
	}
};

