#pragma once
#include <unordered_map>
#include "MobBase.h"
#include "Mobs.h"
#include <string>

class MobPrototypeRegistry {
public:
	void registerPrototype(std::string type, Mob* prototype) {
		prototypes[type] = prototype;
	}

	Mob* getPrototype(std::string type) {
		return prototypes[type];
	}

private:
	std::unordered_map<std::string, Mob*> prototypes;
};

class Renderer;
void InitializeMobPrototypes(MobPrototypeRegistry& registry, Renderer& ren);

class MobFactory {
public:
    MobFactory(Renderer& ren) {
		InitializeMobPrototypes(registry, ren);
	}

	~MobFactory(); // delete prototypes and shared data

    Mob* create(std::string type, glm::vec3 pos) {
        Mob* proto = registry.getPrototype(type);
        Mob* obj = proto->clone(); // Prototype
        obj->instanceData.position = pos;
        return obj;
    }

private:
	MobPrototypeRegistry registry;
};

