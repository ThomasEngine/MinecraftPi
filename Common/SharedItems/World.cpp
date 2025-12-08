#include "World.h"
#include "ChunkLoader.h" 

World::World(Renderer& ren)
{
    // Continentalness Noise
    FastNoiseLite Continentalness = FastNoiseLite(1337);
    Continentalness.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2);
    Continentalness.SetFrequency(0.0008f);
    Continentalness.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
    Continentalness.SetFractalOctaves(3);
    Continentalness.SetFractalGain(0.55f);

    // Erosion noise
    FastNoiseLite Erosion = FastNoiseLite(42);
    Erosion.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    Erosion.SetFrequency(0.002f);

    // Peaks and Valleys noise
    FastNoiseLite PeaksAndValleys = FastNoiseLite(56);
    PeaksAndValleys.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    PeaksAndValleys.SetFrequency(0.005f);
    PeaksAndValleys.SetFractalType(FastNoiseLite::FractalType_Ridged);

    // Caves
    FastNoiseLite CaveNoise = FastNoiseLite(9001);
    CaveNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    CaveNoise.SetFrequency(0.02f);


	m_NoiseMaps = std::make_shared<NoiseMaps>(Continentalness, Erosion, PeaksAndValleys, CaveNoise);
	m_ChunkLoader = std::make_unique<ChunkLoader>(ren, m_NoiseMaps, isReady);
}

World::~World()
{

}

void World::Update(const glm::vec3& camDir, const glm::vec3& camPos, const glm::mat4& viewProjMatrix)
{
	m_ChunkLoader->Update(camDir, camPos, viewProjMatrix);
}

void World::Draw(const glm::mat4 viewProj, Shader& shader, Texture& tex)
{
	m_ChunkLoader->Draw(viewProj, shader, tex);
}

void World::PlaceBlockAtPosition(const glm::vec3& worldPos, const uint8_t& block)
{
	m_ChunkLoader->SetBlockAtPosition(worldPos, block);
}

void World::RemoveBlockAtPosition(const glm::vec3& worldPos)
{
	m_ChunkLoader->RemoveBlockAtPosition(worldPos);
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
