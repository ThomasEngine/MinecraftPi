#pragma once
#include "Renderer.h"
#include "GLM/glm.hpp"

// Data for living entitys like players, mobs, villagers, etc.
struct SharedModelData
{
	Mesh mesh;
};

struct InstanceData {
    glm::vec3 position;
    //State* aiState;
	uint8_t health;
};

class Mob {
public:
    SharedModelData* sharedData; 
    InstanceData instanceData;

    virtual void render() = 0;
	virtual void update(float deltaTime) = 0;
    virtual Mob* clone() = 0;
};


