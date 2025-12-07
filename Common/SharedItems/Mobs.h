#pragma once

#include "MobBase.h"	

class Sheep : public Mob {
public:
	Sheep(SharedModelData* sharedData);
	virtual void render(Renderer&, Shader&, Texture&, glm::mat4 viewProj) override;
	virtual void update(float deltaTime) override;
	virtual Mob* clone() override;
};

class Villager : public Mob {
public:
	Villager(SharedModelData* sharedData);
	virtual void render(Renderer&, Shader&, Texture&, glm::mat4 viewProj) override;
	virtual void update(float deltaTime) override;
	virtual Mob* clone() override;
};