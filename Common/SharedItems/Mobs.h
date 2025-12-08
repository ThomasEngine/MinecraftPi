#pragma once

#include "MobBase.h"	

// Sheep
struct SheepPose {
	glm::vec3 bodyPos, bodyScale;
	glm::vec3 headPos, headScale;
	float headRotation;
	glm::vec3 legPos[4], legScale[4];
	float legRotation[4];
	float walkTime;
};

class Sheep : public Mob {
public:
	Sheep(SharedModelData* sharedData);
	void render(Renderer&, Shader&, Texture&, glm::mat4 viewProj) override;
	void update(float deltaTime) override;
	Mob* clone() override;
		
private:
	SheepPose pose;

	void UpdateChasingBehavior(float deltaTime) override;
	void UpdateMatingBehavior(float deltaTime) override;
	void UpdateWanderingBehavior(float deltaTime) override;

	void UpdateWalkingAnimation(float deltaTime) override;
	void UpdateIdleAnimation(float deltaTime) override;
	void UpdateRunningAnimation(float deltaTime) override;

	void UpdateWanderingAnimation(float deltaTime) override;
	void UpdateChasingAnimation(float deltaTime) override;
	void UpdateMatingAnimation(float deltaTime) override;

	void CheckStateTransition() override;
};


//class Villager : public Mob {
//public:
//	Villager(SharedModelData* sharedData);
//	virtual void render(Renderer&, Shader&, Texture&, glm::mat4 viewProj) override;
//	virtual void update(float deltaTime) override;
//	virtual Mob* clone() override;
//};