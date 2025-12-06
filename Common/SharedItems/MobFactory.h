#pragma once
#include <unordered_map>
#include "MobBase.h"
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


class MobFactory {
public:
    MobFactory(MobPrototypeRegistry& reg) : registry(reg) {}

    Mob* create(std::string type, glm::vec3 pos) {
        Mob* proto = registry.getPrototype(type);
        Mob* obj = proto->clone(); // Prototype
        obj->instanceData.position = pos;
        return obj;
    }

private:
	MobPrototypeRegistry& registry;
};

void InitializeMobPrototypes(MobPrototypeRegistry& registry);
