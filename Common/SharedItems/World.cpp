#include "World.h"
#include "ChunkLoader.h" 
#include "BlockRegistery.h"
#include <ctime> 
#include <Player.h>
#include <MobFactory.h>
#include <Camera.h>
#include <Shader.h>



World::World(Renderer& ren, int seed, Camera* cam)
	: m_Player(cam), m_Renderer(ren)
{
	// Initialize noise maps
    // Continentalness Noise
    FastNoiseLite Continentalness = FastNoiseLite(seed);
    Continentalness.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2);
    Continentalness.SetFrequency(0.0008f);
    Continentalness.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
    Continentalness.SetFractalOctaves(3);
    Continentalness.SetFractalGain(0.55f);

    // Erosion noise
    FastNoiseLite Erosion = FastNoiseLite(seed);
    Erosion.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    Erosion.SetFrequency(0.002f);

    // Peaks and Valleys noise
    FastNoiseLite PeaksAndValleys = FastNoiseLite(seed);
    PeaksAndValleys.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    PeaksAndValleys.SetFrequency(0.005f);
    PeaksAndValleys.SetFractalType(FastNoiseLite::FractalType_Ridged);

    // Caves
    FastNoiseLite CaveNoise = FastNoiseLite(seed);
    CaveNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    CaveNoise.SetFrequency(0.02f);

	m_NoiseMaps = std::make_shared<NoiseMaps>(Continentalness, Erosion, PeaksAndValleys, CaveNoise);

	// Chunk loader
	m_ChunkLoader = std::make_unique<ChunkLoader>(ren, m_NoiseMaps, isReady);

	// Mob Factory
	m_MobFactory = std::make_unique<MobFactory>(ren);

	//// Setup mobs
	//for (int i = 0; i < 10; ++i)
	//{
	//	float x = static_cast<float>(rand() % 256 - 128);
	//	float z = static_cast<float>(rand() % 256 - 128);
	//	float y = 100.0f; // Start high to let them fall to the ground
	//	Mob* m_ = m_MobFactory->create("Sheep", glm::vec3(x, y, z));
	//	m_Mobs.push_back(m_);
	//}

	m_Renderer = ren;
}

World::~World()
{

}

void World::Update(const glm::vec3& camDir, const glm::vec3& camPos, const glm::mat4& viewProjMatrix, float deltaTime)
{
	m_ChunkLoader->Update(camDir, camPos, viewProjMatrix);
	m_Player.Update(deltaTime);
	for (auto& mob : m_Mobs)
	{
		mob->update(deltaTime, m_Player.GetCamera()->GetPosition());
	}
}

void World::Draw(const glm::mat4 viewProj, Shader& shader, Texture& tex)
{
	m_Renderer.startBatch(shader, viewProj, tex);
	m_ChunkLoader->Draw(viewProj, shader, tex);
	shader.Bind();
	shader.SetUniform1f("u_CellWidth", 1.f / 8.f);
	shader.SetUniform1f("u_CellHeight", 1.f / 8.f);
	for (auto& mob : m_Mobs)
	{
		mob->render(m_Renderer, shader, tex, m_Player.GetCamera()->GetViewProjectionMatrix());
	}
	shader.Bind();
	shader.SetUniform1f("u_CellWidth", 1.f / 16.f);
	shader.SetUniform1f("u_CellHeight", 1.f / 16.f);
	m_Renderer.endBatch();
}

void World::PlaceBlockAtPosition(const glm::vec3& worldPos, const uint8_t& block)
{
	m_ChunkLoader->SetBlockAtPosition(worldPos, block);
}

void World::RemoveBlockAtPosition(const glm::vec3& worldPos)
{
	m_ChunkLoader->RemoveBlockAtPosition(worldPos);
}

Mob* World::AddMob(std::unique_ptr<Mob> mob)
{
	m_Mobs.push_back(std::move(mob));
	return m_Mobs.back().get();
}

void World::RemoveMob(Mob* mob)
{
	auto it = std::remove_if(m_Mobs.begin(), m_Mobs.end(), 
		[mob](const std::unique_ptr<Mob>& m) { return m.get() == mob; });
	m_Mobs.erase(it, m_Mobs.end());
}

uint8_t World::GetBlockAtPosition(const glm::vec3& worldPos)
{
	glm::ivec3 chunkPos = WorldToChunkPos(worldPos);
	glm::vec3 position = glm::vec3(
		int(std::floor(worldPos.x)) - chunkPos.x * 16,
		int(std::floor(worldPos.y)),
		int(std::floor(worldPos.z)) - chunkPos.z * 16
	);
	return m_ChunkLoader->GetBlockAtPosition(position, chunkPos);
}

glm::vec3 World::WorldToChunkPos(const glm::vec3& pos) const
{
	return m_ChunkLoader->WorldToChunkPos(pos);
}

glm::ivec3 World::vec3ToIvec3(const glm::vec3& vec) const  
{  
   return glm::ivec3(  
       int(floor(vec.x)),  
       int(floor(vec.y)),  
       int(floor(vec.z))  
   );  
}

void World::SetCollisionSystem(std::shared_ptr<CollisionSystem> cs)
{
	// Assign collision system to player and world
	m_Player.SetCollisionSystem(cs);
	m_CollisionSystem = cs;
	m_CollisionSystem.get()->SetBlockTarget(*this);
}
