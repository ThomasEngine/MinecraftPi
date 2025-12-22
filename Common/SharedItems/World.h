#pragma once  
#include "Renderer.h"  
#include "noise/FastNoiseLite.h"
#include <memory>
#include "Player.h"

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

constexpr float GROUND_ACCEL = 35.0f; 
constexpr float AIR_ACCEL = 8.0f;   
constexpr float WATER_ACCEL = 4.0f;
constexpr float GROUND_FRICTION = 6.0f;    
constexpr float AIR_FRICTION = 0.0f;
constexpr float WATER_FRICTION = 3.0f;
constexpr float MAX_GROUND_SPEED = 5.5f;   
constexpr float MAX_AIR_SPEED = 5.5f;    
constexpr float MAX_WATER_SPEED = 3.0f;
constexpr float MAX_FALL_SPEED = 78.4f;   
constexpr float JUMP_SPEED = 8.4f;   
constexpr float GRAVITY = 32.0f;   
constexpr float SPRINT_MULTIPLIER = 1.3f;   
constexpr float CROUCH_MULTIPLIER = 0.4f;

class ChunkLoader;  
class FastNoiseLite;  
class Mob;
class MobFactory;
class World  
{  
public:  
	World(Renderer& ren, int seed, Camera* cam);  
	~World();  

	void Update(const glm::vec3& camDir, const glm::vec3& camPos, const glm::mat4& vpm, float dt);  
	void Draw(const glm::mat4 viewProj, Shader& shader, Texture& tex);

	void PlaceBlockAtPosition(const glm::vec3& worldPos, const uint8_t& block);  
	void RemoveBlockAtPosition(const glm::vec3& worldPos);  
	Mob* AddMob(std::unique_ptr<Mob> mob);
	void RemoveMob(Mob* mob);

	uint8_t GetBlockAtPosition(const glm::vec3& position);

	glm::vec3 WorldToChunkPos(const glm::vec3& pos) const;
	glm::ivec3 vec3ToIvec3(const glm::vec3& vec) const;

	bool GetReady() const { return isReady; }

	void SetCollisionSystem(std::shared_ptr<CollisionSystem> cs);
	Player& GetPlayer() { return m_Player; }
private:  
	// List of noise maps
	std::shared_ptr<NoiseMaps> m_NoiseMaps;
	// Chunk loader  
	std::unique_ptr<ChunkLoader> m_ChunkLoader;

	Renderer& m_Renderer;
	
	Player m_Player;
	std::unique_ptr<MobFactory> m_MobFactory;
	std::vector<std::unique_ptr<Mob>> m_Mobs;
	std::shared_ptr<CollisionSystem> m_CollisionSystem;

	bool isReady{ false };
};
