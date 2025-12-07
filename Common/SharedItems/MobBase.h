#pragma once
#include "Renderer.h"
#include "GLM/glm.hpp"
#include "CollisionSystem.h"

// Data for living entitys like players, mobs, villagers, etc.
struct SharedModelData
{
	Mesh bodyMesh;
	Mesh headMesh;
	Mesh legMesh[4]; 
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

struct InstanceData {
	glm::vec3 position;
	AiState aiState;
	uint8_t health;
	glm::vec3 direction;
	float speed;
	glm::vec3 moveTarget;
	bool hasMovetarget = false;
	std::vector<glm::vec3> path;
	uint8_t pathIndex = 0;
	uint8_t lastIndexChecked = 0;
};

class Mob {
public:
    SharedModelData* sharedData; 
    InstanceData instanceData;

    virtual void render(Renderer&, Shader&, Texture&, glm::mat4 viewProj) = 0;
	virtual void update(float deltaTime) = 0;

    virtual Mob* clone() = 0;
	void setPosition(const glm::vec3& pos) {
		instanceData.position = pos;
	}

	void moveTo(const glm::vec3& target);

	virtual void UpdateWanderingBehavior(float deltaTime) = 0;
	virtual void UpdateChasingBehavior(float deltaTime) = 0;
	virtual void UpdateMatingBehavior(float deltaTime) = 0;

	virtual void UpdateWalkingAnimation(float deltaTime) = 0;
	virtual void UpdateChasingAnimation(float deltaTime) = 0;
	virtual void UpdateMatingAnimation(float deltaTime) = 0;

	virtual void CheckStateTransition() = 0;
protected:
	CollisionSystem* m_CS;

	void FindPath(const glm::vec3& target);

	void UpdateMovement(float deltaTime);


	void UpdateBehavior(float deltaTime);
	void UpdateAnimation(float deltaTime);

};


