#pragma once
#include "MobBase.h"	

// FourlegMob

class FourlegMob : public Mob {
public:
	FourlegMob(SharedModelData* sharedData);
	void render(Renderer&, Shader&, Texture&, glm::mat4 viewProj) override;
	void update(float deltaTime, const glm::vec3& playerPos) override;
	Mob* clone() override;
		
private:
	void UpdateChasingBehavior(float deltaTime) override;
	void UpdateMatingBehavior(float deltaTime) override;
	void UpdateWanderingBehavior(float deltaTime) override;

	void CheckStateTransition() override;
};


//class Villager : public Mob {
//public:
//	Villager(SharedModelData* sharedData);
//	virtual void render(Renderer&, Shader&, Texture&, glm::mat4 viewProj) override;
//	virtual void update(float deltaTime) override;
//	virtual Mob* clone() override;
//};