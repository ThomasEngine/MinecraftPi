#pragma once  
#include "Rendering/include/Renderer.h"  
#include "FastNoiseLite.h"

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


class ChunkLoader;  
class FastNoiseLite;  
class World  
{  
public:  
	World(Renderer& ren);  
	~World();  

	void Update(const glm::vec3& camDir, const glm::vec3& camPos, const glm::mat4& viewProjMatrix);  
	void Draw(const glm::mat4 viewProj, Shader& shader, Texture& tex);

	void PlaceBlockAtPosition(const glm::vec3& worldPos);  
	void RemoveBlockAtPosition(const glm::vec3& worldPos);  
private:  
	// List of noise maps
	std::shared_ptr<NoiseMaps> m_NoiseMaps;

	// Chunk loader  
	std::unique_ptr<ChunkLoader> m_ChunkLoader; // Ensure std::unique_ptr is included  
};
