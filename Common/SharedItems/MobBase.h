#pragma once
#include "Renderer.h"
#include "GLM/glm.hpp"
#include "CollisionSystem.h"

struct SharedModelData
{
	Mesh bodyMesh;
	Mesh headMesh;
	Mesh legMesh[4]; 
	glm::vec3 hitbox;
	glm::vec3 legTopPosition[4];
};

enum Legs {
	FL = 0,
	FR,
	BL,
	BR 
};

enum AiState {
	Wandering,
	Chasing,
	Mating
};

enum WalkingState {
	Idle,
	Walking,
	Running
};

class PoseBase {
public:
	glm::vec3 bodyPos, bodyScale;
	glm::vec3 headPos, headScale;
	float headRotation;
	glm::vec3 legPos[4], legScale[4];
	float legRotation[4];
	float walkTime;
	float bodyRotation;

	virtual void walkingAnimation(float deltaTime) = 0;
	virtual void idleAnimation(float deltaTime) = 0;
	virtual void updateHeadMovement(float deltaTime, const glm::vec3& playerPos);
	virtual void updateBodyRotation(float targetAngle);
};

class AllFourAnimations : public PoseBase {
public:
	void walkingAnimation(float deltaTime) override;
	void idleAnimation(float deltaTime) override;

};

struct InstanceData {
	glm::vec3 position;
	AiState aiState;
	WalkingState walkState;
	uint8_t health;
	glm::vec3 direction;
	glm::vec3 velocity;
	float speed;
	glm::vec3 moveTarget;
	bool hasMovetarget = false;
	std::vector<glm::vec3> path;
	uint8_t pathIndex = 0;
	uint8_t lastIndexChecked = 0;
	bool onGround = true;
	float timer = 0.0f;
	float waitTime = 5.0f;
};

class Mob {
public:
    SharedModelData* sharedData; 
    InstanceData instanceData;

    virtual void render(Renderer&, Shader&, Texture&, glm::mat4 viewProj) = 0;
	virtual void update(float deltaTime, const glm::vec3& playerPos) = 0;

    virtual Mob* clone() = 0;
	void setPosition(const glm::vec3& pos) {
		instanceData.position = pos;
	}
	void Move(float deltaTime);

	void moveTo(const glm::vec3& target);
	void Jump(float deltaTime);

	virtual void UpdateWanderingBehavior(float deltaTime) = 0;
	virtual void UpdateChasingBehavior(float deltaTime) = 0;
	virtual void UpdateMatingBehavior(float deltaTime) = 0;

	virtual void UpdateWanderingAnimation(float deltaTime);
	virtual void UpdateChasingAnimation(float deltaTime);
	virtual void UpdateMatingAnimation(float deltaTime);

	virtual void CheckStateTransition() = 0;
	void SetCollisionSystem(CollisionSystem* cs) { m_CS = cs; }
	void UpdateAnimation(float deltaTime);
protected:
	CollisionSystem* m_CS;
	PoseBase* pose;

	void GetRandomWanderTarget();
	void FindPath(const glm::vec3& target);

	void UpdateBehavior(float deltaTime, const glm::vec3& playerPos);

private:

	void UpdateMovement(float deltaTime);
	void UpdateYMovement(float deltaTime);

};


