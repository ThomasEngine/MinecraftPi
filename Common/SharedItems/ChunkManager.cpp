#include "ChunkManager.h"
#include "Chunk.h"
#include "algorithm"
#include <noise/FastNoiseLite.h>

constexpr int CHUNK_RADIUS = 8;

ChunkManager::ChunkManager(Renderer& rend)
    : m_CameraPos(0.0f), m_CameraDir(0.0f)
{
    FNL = new FastNoiseLite(1337); // seed
    FNL->SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Perlin);

    for (int x = -CHUNK_RADIUS; x < CHUNK_RADIUS; x++)
    {
        for (int z = -CHUNK_RADIUS; z < CHUNK_RADIUS; z++)
        {
            Chunk* chunk = new Chunk(glm::ivec3(x, 0, z), *FNL);
            m_SetupList.push_back(chunk);
			m_Chunks.emplace(glm::ivec3(x, 0, z), chunk);
        }
    }

    for (auto& c : m_SetupList)
    {
        c->createChunkMesh(rend, *this);
    }
}

ChunkManager::~ChunkManager()
{
    delete FNL;
}

void ChunkManager::Update(float dt, const glm::vec3 camPos, const glm::vec3 camDir, Renderer& renderer)
{
}

void ChunkManager::Draw(Renderer& renderer, const glm::mat4 viewProj, Shader& shader, Texture& tex)
{
    for (Chunk* chunk : m_SetupList)
    {
        if (chunk->isReady)
        {
            chunk->Draw(renderer, viewProj, shader, tex);
        }
    }
}

uint8_t ChunkManager::GetBlockAtPosition(const glm::vec3 position, const glm::ivec3 chunkPos)
{
    auto it = m_Chunks.find(chunkPos);
    if (it == m_Chunks.end() || it->second == nullptr)
        return 0;
    Chunk* chunk = it->second;
    return chunk->GetBlock(position.x, position.y, position.z);
}

void ChunkManager::UpdateLoadList(const glm::vec3 camPos)
{
}

void ChunkManager::UpdateUnloadList()
{
}

void ChunkManager::UpdateSetupList(Renderer& renderer)
{
}

void ChunkManager::UpdateRebuildList(Renderer& renderer)
{
}

void ChunkManager::UpdateVisibilityList(const glm::vec3 camPos, const glm::vec3 camDir)
{
}

void ChunkManager::UpdateRenderList()
{
}
