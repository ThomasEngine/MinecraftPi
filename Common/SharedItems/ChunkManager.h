#pragma once
#include <Renderer.h>
#include <map>

struct IVec3Less {
	bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
		if (a.x != b.x) return a.x < b.x;
		if (a.y != b.y) return a.y < b.y;
		return a.z < b.z;
	}
};


class Shader;
class Texture;
class Chunk;
class FastNoiseLite;
class ChunkManager
{
public:
	ChunkManager(Renderer& rend);
	~ChunkManager();

	void Update(float dt, const glm::vec3 camPos, const glm::vec3 comDir, Renderer& renderer);
	void Draw(Renderer& renderer, const glm::mat4 viewProj, Shader& shader, Texture& tex);

	uint8_t GetBlockAtPosition(const glm::vec3 position, const glm::ivec3 chunkPos);

private:
	glm::ivec2 m_LastCameraChunk;
	glm::vec3 m_CameraPos, m_CameraDir;
	std::vector<Chunk*> m_LoadList, m_SetupList, m_RebuildList, m_UnloadList, m_VisibilityList, m_RenderList;

	FastNoiseLite* FNL;
	std::map<glm::ivec3, Chunk*, IVec3Less> m_Chunks;

	void UpdateLoadList(const glm::vec3 camPos);
	void UpdateSetupList(Renderer& renderer);
	void UpdateRebuildList(Renderer& renderer);
	void UpdateUnloadList();
	void UpdateVisibilityList(const glm::vec3 camPos, const glm::vec3 camDir);
	void UpdateRenderList();
};

