#pragma once
#include <Renderer.h>
#include <map>
#include <queue>
#include <set>
#include <FastNoiseLite.h>
#include <future>

struct IVec3Less {
	bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
		if (a.x != b.x) return a.x < b.x;
		if (a.y != b.y) return a.y < b.y;
		return a.z < b.z;
	}
};

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

private:
	static constexpr int VIEW_DISTANCE = 7;

	glm::vec3 m_CameraPos, m_CameraDir;

	FastNoiseLite m_Continentalness;
	FastNoiseLite m_Erosion;
	FastNoiseLite m_PeaksAndValleys;


	std::map<glm::ivec3, Chunk*, IVec3Less> m_Chunks;
	std::vector<std::future<std::unique_ptr<Chunk>>> m_ChunkFutures;
	std::vector<Chunk*> m_RenderList;
	std::vector<Chunk*> m_InShotRenderList;
	std::vector<Chunk*> m_PendingMesh;

	std::queue<glm::ivec3> m_ChunksToLoad;
	std::queue<glm::ivec3> m_ChunksToUnload;
	std::set<glm::ivec3, IVec3Less> m_ChunksScheduledForLoad;
	std::set<glm::ivec3, IVec3Less> m_ChunksScheduledForUnload;

	glm::ivec3 WorldToChunkPos(const glm::vec3& pos) const;
	bool AreNeighborsLoaded(const glm::ivec3& pos) const;
	void FindChunksToLoadAndUnload(const glm::vec3& camPos);
	void ProcessChunkLoading(Renderer& renderer);
	void ProcessChunkUnloading(Renderer& renderer);
	void UpdateInShotRenderList(const glm::mat4& viewProj);
};