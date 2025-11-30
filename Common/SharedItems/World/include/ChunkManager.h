#pragma once
#include <Rendering/include/Renderer.h>
#include <map>
#include <queue>
#include <set>
#include <FastNoiseLite.h>
#include "Camera/include/Frustum.h"

struct IVec3Less {
	bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
		if (a.x != b.x) return a.x < b.x;
		if (a.y != b.y) return a.y < b.y;
		return a.z < b.z;
	}
};

struct IVec3Hash {
	std::size_t operator()(const glm::ivec3& v) const noexcept {
		std::size_t h1 = std::hash<int>()(v.x);
		std::size_t h2 = std::hash<int>()(v.y);
		std::size_t h3 = std::hash<int>()(v.z);
		return h1 ^ (h2 << 1) ^ (h3 << 2);
	}
};

struct ChunkLoadTask {
	glm::ivec3 chunkPos;
	std::shared_ptr<Chunk> chunk;
	bool pendingMesh = false;
	bool pendingSunlight = true;
	bool pendingSunlightFill = false;
	bool renderReady = false;
};

extern const glm::ivec3 offsets[];

class Camera;
class Shader;
class Texture;
class Chunk;
class FastNoiseLite;
class ChunkManager
{
public:
	ChunkManager(Renderer& rend);
	~ChunkManager();

	void Update(const Camera& cam, Renderer& renderer);
	void Draw(Renderer& renderer, const glm::mat4 viewProj, Shader& shader, Texture& tex);

	uint8_t GetBlockAtPosition(const glm::vec3& position, const glm::ivec3& chunkPos);
	uint8_t GetBlockAtPosition(const glm::vec3& worldPos);

	void PlaceBlockAtPosition(const glm::vec3& worldPos);

	Chunk* GetChunk(const glm::ivec3& chunkPos);
	void SetBlockLightLevel(const glm::ivec3& worldPos, uint8_t lightLevel);
	uint8_t GetBlockLightLevel(const glm::ivec3& worldPos);
	bool AreNeighborsLoaded(const glm::ivec3& pos) const;
	glm::ivec3 WorldToChunkPos(const glm::vec3& pos) const;
	
	// Overworld noise generators
	FastNoiseLite m_Continentalness;
	FastNoiseLite m_Erosion;
	FastNoiseLite m_PeaksAndValleys;

	// Cave noise
	FastNoiseLite m_CaveNoise;

private:
	uint8_t VIEW_DISTANCE = 8;
	uint8_t HALF_X = 0;
	uint8_t HALF_Z = 0;
	uint8_t HALF_Y = 0;

	Frustum frustum;

	glm::vec3 m_CameraPos, m_CameraDir;

	std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>, IVec3Hash> m_Chunks;
	std::unordered_map<glm::ivec3, ChunkLoadTask, IVec3Hash> m_ChunkLoadTasks;
	std::vector<std::shared_ptr<Chunk>> m_RenderList;
	std::vector<std::shared_ptr<Chunk>> m_InShotRenderList;

	std::queue<glm::ivec3> m_ChunksToLoad;
	std::queue<glm::ivec3> m_ChunksToUnload;
	std::set<glm::ivec3, IVec3Less> m_ChunksScheduledForLoad;
	std::set<glm::ivec3, IVec3Less> m_ChunksScheduledForUnload;

	void FindChunksToLoadAndUnload(const glm::vec3& camPos);
	void ProccessChunkLoadingAsync(Renderer& renderer);
	void ProcessChunkLoading(Renderer& renderer);
	void ProcessChunkUnloading(Renderer& renderer);
	void UpdateInShotRenderList(const glm::mat4& viewProj);
};