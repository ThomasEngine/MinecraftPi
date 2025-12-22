#pragma once
#include "glm/glm.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

enum class MobCategory { Monster = 0, Animal = 1, Structure = 2, Count = 3};

struct MobSpawnGroup {
	unsigned int minCount;
	unsigned int maxCount;
};

struct MobCab {
	int currentCount;
	int maxCount;
};

struct MobSpawnRatio {
	unsigned int min;
	unsigned int max;
};

class Mob;
class World;
class Renderer;
class Shader;
class MobFactory;
class MobLoader
{
public:
	MobLoader(World* world, Renderer& ren);
	~MobLoader();

	void Update(const glm::vec3& playerPos, float dt);
private:
	std::unique_ptr<MobFactory> m_MobFactory;
	std::vector<Mob*> m_Mobs[(int)MobCategory::Count];
	std::unordered_map<MobCategory, MobCab> m_MobCabs;

	MobSpawnGroup m_SpawnGroups[(int)MobCategory::Count];

	World* m_Owner;

	int m_TickCounter;
	float m_TimeSinceLastUpdate;
	bool m_Ready = { false };

	MobSpawnRatio m_NaturalSpawnRatio;

	void InitializeMobCabs();

	void UpdateMobCabs();
	void TryNaturalSpawn(const glm::vec3& playerPos);
	void DespawnFarMobs(const glm::vec3& playerPos);

	bool canMobLoad(glm::vec3& mobPos);

	void MobTikkUpdate(float deltaTime);
};

