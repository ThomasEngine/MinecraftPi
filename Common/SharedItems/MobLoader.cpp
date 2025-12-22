#include "MobLoader.h"
#include "MobFactory.h"
#include "World.h"

MobLoader::MobLoader(std::shared_ptr<World> world)
{
	m_MobFactory = std::make_unique<MobFactory>();

}

MobLoader::~MobLoader()
{
}

void MobLoader::Update(const glm::vec3& playerPos, float deltaTime)
{
	// Load and unload mobs
	LoadMobs(playerPos);
	UnloadMobs(playerPos);

	for (auto& mob : m_Mobs)
	{
		mob->update(deltaTime, playerPos); 	
	}
}

void MobLoader::Render(Renderer& ren, Shader& sh, const glm::mat4& viewProj)
{
	for (auto& mob : m_Mobs)
	{
		mob->render(ren, sh, Texture() viewProj);
	}
}

void MobLoader::InitializeMobCabs()
{
	
}

void MobLoader::UpdateMobCabs()
{
}

void MobLoader::TryNaturalSpawn(const glm::vec3& playerPos)
{
	
}

void MobLoader::DespawnFarMobs(const glm::vec3& playerPos)
{
	auto it = m_Mobs.begin();
	while (it != m_Mobs.end()) {
		Mob* mob = *it;
		if (glm::length(playerPos - mob->instanceData.position) > m_NaturalSpawnRatio.max) {
			m_Owner->RemoveMob(mob); 
			it = m_Mobs.erase(it);  		
		}
		else {
			++it;
		}
	}
}

void MobLoader::LoadMobs(const glm::vec3& playerPos)
{
	// When loading new chunks around the player, spawn mobs at random positions within those chunks

}

void MobLoader::UnloadMobs(const glm::vec3& playerPos)
{
}

bool MobLoader::canMobLoad(const glm::vec3& mobPos, const glm::vec3& playerPos)
{
	// if mob is not in water and within 100 units of player
	


	return false;
}
