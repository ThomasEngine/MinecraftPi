#pragma once
#include "glm/glm.hpp"
#include <memory>
#include <vector>

enum class MobCategory { Monster, Animal, Structure, Count};

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
	MobLoader(std::shared_ptr<World>);
	~MobLoader();

	void Update(const glm::vec3& playerPos, float dt);
	void Render(Renderer& ren, Shader& sh, const glm::mat4& viewProj);

private:
	std::unique_ptr<MobFactory> m_MobFactory;
	std::vector<Mob*> m_Mobs;

	std::shared_ptr<World> m_Owner;

	std::unordered_map<MobCategory, MobCab> m_MobCabs;
	int m_TickCounter;
	MobSpawnRatio m_NaturalSpawnRatio;

	void InitializeMobCabs();

	void UpdateMobCabs();
	void TryNaturalSpawn(const glm::vec3& playerPos);
	void DespawnFarMobs(const glm::vec3& playerPos);

	void LoadMobs(const glm::vec3& playerPos);
	void UnloadMobs(const glm::vec3& playerPos);

	bool canMobLoad(const glm::vec3& mobPos, const glm::vec3& playerPos);
};

