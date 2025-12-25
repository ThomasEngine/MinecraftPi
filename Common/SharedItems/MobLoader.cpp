#include "MobLoader.h"
#include "MobFactory.h"
#include "World.h"
#include "BlockRegistery.h"

MobLoader::MobLoader(World* world, Renderer& ren)
{
	m_MobFactory = std::make_unique<MobFactory>(ren);
	m_Owner = world;
	m_TickCounter = 0;

	// Setup spawn groups
	m_SpawnGroups[(int)MobCategory::Monster] = { 1, 4 };
	m_SpawnGroups[(int)MobCategory::Animal] = { 2, 5 };
	m_SpawnGroups[(int)MobCategory::Structure] = { 1, 2 };
	m_NaturalSpawnRatio = { 2 * 16, 6 * 16 };

	InitializeMobCabs();
}

MobLoader::~MobLoader()
{
}

void MobLoader::Update(const glm::vec3& playerPos, float deltaTime)
{
	if (!m_Ready) { m_Ready = m_Owner->GetReady(); return; }
	// Load and unload mobs
	// Should only happen 20 times a second
	m_TimeSinceLastUpdate += deltaTime;
	if (m_TimeSinceLastUpdate >= 0.05f)
	{
		// Spawn new mobs if under cap
		TryNaturalSpawn(playerPos);
		// Despawn far mobs
		DespawnFarMobs(playerPos);

		// Update mob cab counts
		UpdateMobCabs();
		m_TimeSinceLastUpdate = 0.0f;
	}
}



void MobLoader::InitializeMobCabs()
{
	// Setup mob cab max counts
	m_MobCabs[MobCategory::Monster] = { 0, 10 };
	m_MobCabs[MobCategory::Animal] = { 0, 20 };
	m_MobCabs[MobCategory::Structure] = { 0, 5 };
}

void MobLoader::UpdateMobCabs()
{
	// Update current counts
	for (int i = 0; i < (int)MobCategory::Count; i++)
	{
		m_MobCabs[(MobCategory)i].currentCount = m_Mobs[i].size();
	}
}

void MobLoader::TryNaturalSpawn(const glm::vec3& playerPos)
{
	// Check if we can spawn more mobs based on the natural spawn ratio
	// Determine if want to spawn zombie or passive mob based on random chance
	MobCategory mobToSpawn = (rand() % 2 == 0) ? MobCategory::Monster : MobCategory::Animal;
	// Only have animals now so will make it always Animal
	mobToSpawn = MobCategory::Animal;

	MobCab& it = m_MobCabs[mobToSpawn];
	it.currentCount = m_Mobs[(int)mobToSpawn].size();
	if (it.currentCount < it.maxCount)
	{
		// Add a get Random mob from category
		// After that determine how many random of mob type should spawn
		const MobSpawnGroup& it = m_SpawnGroups[(int)mobToSpawn];
		int mobsToSpawn = rand() % it.maxCount;
		mobsToSpawn = mobsToSpawn > it.minCount ? mobsToSpawn : it.minCount;

		// GetPlacement of the first mob to spawn
		float mX = (m_NaturalSpawnRatio.min + rand() % (m_NaturalSpawnRatio.max - m_NaturalSpawnRatio.min)) ;
		float mZ = (m_NaturalSpawnRatio.min + rand() % (m_NaturalSpawnRatio.max - m_NaturalSpawnRatio.min)) ;
		if (rand() % 2 == 0) mX = -mX;
		if (rand() % 2 == 0) mZ = -mZ;
		glm::vec3 mobPos = glm::vec3(playerPos.x + mX, playerPos.y + 50.0f, playerPos.z + mZ);

		// Check if can spawn at position
		mobPos = m_Owner->GetYofXZ(mobPos);
		if (mobPos == glm::vec3(-1.f))
			return;
		if (!canMobLoad(mobPos))
			return;

		// Spawn mobs
		for (int i = 0; i < mobsToSpawn; ++i)
		{
			// Slightly vary spawn position
			float offsetX = (rand() % 20) - 10;
			float offsetZ = (rand() % 20) - 10;
			glm::vec3 spawnPos = mobPos + glm::vec3(offsetX, 0.0f, offsetZ);
			// Check if can spawn at position
			if (!canMobLoad(spawnPos))
				continue;
			// Create mob and add to world
			Mob* newMob = m_MobFactory->create("Sheep", spawnPos);
			m_Owner->AddMob(std::unique_ptr<Mob>(newMob));
			m_Mobs[(int)mobToSpawn].push_back(newMob);
		}
	}
}

void MobLoader::DespawnFarMobs(const glm::vec3& playerPos)
{
	// Despawn mobs that are too far from the player
	// Check each mob category
	for (int i = 0; i < (int)MobCategory::Count; i++)
	{
		// Iterate through mobs in category
		auto it = m_Mobs[i].begin();

		while (it != m_Mobs[i].end()) {
			Mob* mob = *it;
			if (glm::length(playerPos - mob->instanceData.position) > m_NaturalSpawnRatio.max) { // Despawn distance
				m_Owner->RemoveMob(mob);
				it = m_Mobs[i].erase(it);
			}
			else {
				++it;
			}
		}
	}

}

bool MobLoader::canMobLoad(glm::vec3& mobPos)
{
	// Check if block at position is grass
	glm::vec3 checkPos = mobPos;
	checkPos.y -= 1;
	if (m_Owner->GetBlockAtPosition(checkPos) == B_GRASS)
		return true;
	return false;
}

void MobLoader::MobTikkUpdate(float deltaTime)
{
	// Update mob lighting and behavior once 20 times a second

}
