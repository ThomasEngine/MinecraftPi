#pragma once  
#include "Renderer.h"  
#include "noise/FastNoiseLite.h"

struct NoiseMaps {
	NoiseMaps(
		FastNoiseLite continentalnessNoise,
		FastNoiseLite erosionNoise,
		FastNoiseLite peaksAndValleysNoise,
		FastNoiseLite caveNoiseMap)
		: continentalness(continentalnessNoise),
		erosion(erosionNoise),
		peaksAndValleys(peaksAndValleysNoise),
		caveNoise(caveNoiseMap)
	{
	}
	FastNoiseLite continentalness;
	FastNoiseLite erosion;
	FastNoiseLite peaksAndValleys;
	FastNoiseLite caveNoise;
};

constexpr float GRAVITY = 20.f;


class ChunkLoader;  
class FastNoiseLite;  
class World  
{  
public:  
	World(Renderer& ren);  
	~World();  

	void Update(const glm::vec3& camDir, const glm::vec3& camPos, const glm::mat4& viewProjMatrix);  
	void Draw(const glm::mat4 viewProj, Shader& shader, Texture& tex);

	void PlaceBlockAtPosition(const glm::vec3& worldPos, const uint8_t& block);  
	void RemoveBlockAtPosition(const glm::vec3& worldPos);  

	uint8_t GetBlockAtPosition(const glm::vec3& position);

	glm::vec3 WorldToChunkPos(const glm::vec3& pos) const;
	glm::ivec3 vec3ToIvec3(const glm::vec3& vec) const;
private:  
	// List of noise maps
	std::shared_ptr<NoiseMaps> m_NoiseMaps;

	// Chunk loader  
	std::unique_ptr<ChunkLoader> m_ChunkLoader;
};
