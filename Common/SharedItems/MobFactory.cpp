#include "MobFactory.h"
#include "Mobs.h"

namespace {
	Mesh LoadSheepModel() {
		Mesh mesh;
		// Load mesh data for sheep
		return mesh;
	}

	Mesh LoadVillagerModel() {
		Mesh mesh;
		// Load mesh data for villager
		return mesh;
	}
}


void InitializeMobPrototypes(MobPrototypeRegistry& registry)
{
	SharedModelData* sheepModel = new SharedModelData();
	sheepModel->mesh = LoadSheepModel();
	registry.registerPrototype("Sheep", new Sheep(sheepModel));


	SharedModelData* villagerModel = new SharedModelData();
	villagerModel->mesh = LoadVillagerModel();
	registry.registerPrototype("Villager", new Villager(villagerModel));
}
