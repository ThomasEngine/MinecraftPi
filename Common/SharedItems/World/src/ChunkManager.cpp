#include "World/include/ChunkManager.h"
#include "World/include/Chunk.h"
#include <cmath>
#include "Camera/include/Frustum.h"
#include "Camera/include/Camera.h"

static const glm::ivec3 offsets[] = {
    { 1, 0, 0 }, { -1, 0, 0 },
    { 0, 0, 1 }, { 0, 0, -1 }
};

ChunkManager::ChunkManager(Renderer& rend)
    : m_CameraPos(-1.0f), m_CameraDir(0.0f)
{
	// Continentalness Noise
    m_Continentalness = FastNoiseLite(1337);
    m_Continentalness.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2);
    m_Continentalness.SetFrequency(0.0008f);
    m_Continentalness.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
    m_Continentalness.SetFractalOctaves(3);
    m_Continentalness.SetFractalGain(0.55f);

	// Erosion noise
	m_Erosion = FastNoiseLite(42);
    m_Erosion.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_Erosion.SetFrequency(0.002f); 

	// Peaks and Valleys noise
    m_PeaksAndValleys = FastNoiseLite(56);
    m_PeaksAndValleys.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_PeaksAndValleys.SetFrequency(0.003f);
    m_PeaksAndValleys.SetFractalType(FastNoiseLite::FractalType_Ridged);
}


ChunkManager::~ChunkManager()
{
    for (auto& pair : m_Chunks) {
        delete pair.second;
    }
}

glm::ivec3 ChunkManager::WorldToChunkPos(const glm::vec3& pos) const
{
    return glm::ivec3(
        int(std::floor(pos.x / CHUNK_SIZE_X)),
        0,
        int(std::floor(pos.z / CHUNK_SIZE_Z))
    );
}

void ChunkManager::Update(const Camera& cam, Renderer& renderer)
{
    m_CameraDir = cam.GetDirection();
	glm::vec3 camPos = cam.GetPosition();
    if (WorldToChunkPos(m_CameraPos) != WorldToChunkPos(camPos))
    {
        m_CameraPos = camPos;
        FindChunksToLoadAndUnload(camPos);
    }
    else
    {
		m_CameraPos = camPos;
    }

    if (!m_ChunksToLoad.empty()) {
        ProcessChunkLoading(renderer);
    }
    else if (!m_ChunksToUnload.empty()) {
        ProcessChunkUnloading(renderer);
    }
    else if (!m_PendingMesh.empty()) {
        // Try to mesh one pending chunk per frame
        for (auto it = m_PendingMesh.begin(); it != m_PendingMesh.end(); ++it) {
            Chunk* chunk = *it;
            if (AreNeighborsLoaded(chunk->chunkPos)) {
                chunk->createChunkMesh(renderer, *this);
                m_PendingMesh.erase(it);
                break; // Only one per frame
            }
        }
    }
    UpdateInShotRenderList(cam.GetViewProjectionMatrix());
}

bool ChunkManager::AreNeighborsLoaded(const glm::ivec3& pos) const {

    for (const auto& off : offsets) {
        if (m_Chunks.find(pos + off) == m_Chunks.end())
            return false;
    }
    return true;
}


void ChunkManager::FindChunksToLoadAndUnload(const glm::vec3& camPos)
{
    glm::ivec3 camChunk = WorldToChunkPos(camPos);

    // Mark all chunks within view distance
    std::set<glm::ivec3, IVec3Less> neededChunks;
    for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; ++dx) {
        for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; ++dz) {
            glm::ivec3 pos = camChunk + glm::ivec3(dx, 0, dz);
            neededChunks.insert(pos);
        }
    }

    // loads
    for (const auto& pos : neededChunks) {
        if (m_Chunks.find(pos) == m_Chunks.end() && m_ChunksScheduledForLoad.find(pos) == m_ChunksScheduledForLoad.end()) {
            m_ChunksToLoad.push(pos);
            m_ChunksScheduledForLoad.insert(pos);
        }
    }

    // unloads
    for (const auto& pair : m_Chunks) {
        if (neededChunks.find(pair.first) == neededChunks.end() && m_ChunksScheduledForUnload.find(pair.first) == m_ChunksScheduledForUnload.end()) {
            m_ChunksToUnload.push(pair.first);
            m_ChunksScheduledForUnload.insert(pair.first);
        }
    }
}

void ChunkManager::ProcessChunkLoading(Renderer& renderer)
{
    if (m_ChunksToLoad.empty()) return;
    glm::ivec3 pos = m_ChunksToLoad.front();
    m_ChunksToLoad.pop();
    m_ChunksScheduledForLoad.erase(pos);

    Chunk* chunk = new Chunk(pos, m_Continentalness, m_Erosion, m_PeaksAndValleys);
    m_Chunks[pos] = chunk;
    m_RenderList.push_back(chunk);

    // Mmesh creation if neighbors not loaded
    if (AreNeighborsLoaded(pos)) {
        chunk->createChunkMesh(renderer, *this);
    }
    else {
        m_PendingMesh.push_back(chunk);
    }
}


void ChunkManager::ProcessChunkUnloading(Renderer& renderer)
{
    if (m_ChunksToUnload.empty()) return;
    glm::ivec3 pos = m_ChunksToUnload.front();
    m_ChunksToUnload.pop();
    m_ChunksScheduledForUnload.erase(pos);

    auto it = m_Chunks.find(pos);
    if (it != m_Chunks.end()) {
        Chunk* chunk = it->second;
        chunk->destroyMesh(renderer);
        delete chunk;
        m_Chunks.erase(it);
        
        // Remove from render list
        m_RenderList.erase(std::remove(m_RenderList.begin(), m_RenderList.end(), chunk), m_RenderList.end());
    }
}

void ChunkManager::UpdateInShotRenderList(const glm::mat4& viewProj)
{
    m_InShotRenderList.clear();
    static Frustum frustum;
    frustum.Extract(viewProj);

    for (Chunk* chunk : m_RenderList) {
        glm::vec3 center = glm::vec3(
            (chunk->chunkPos.x + 0.5f) * CHUNK_SIZE_X,
            CHUNK_SIZE_Y / 2.0f,
            (chunk->chunkPos.z + 0.5f) * CHUNK_SIZE_Z
        );
        static const float halfX = CHUNK_SIZE_X / 2.0f;
        static const float halfY = CHUNK_SIZE_Y / 2.0f;
        static const float halfZ = CHUNK_SIZE_Z / 2.0f;

        if (frustum.BoxInFrustum(center, halfX, halfY, halfZ)) {
            m_InShotRenderList.push_back(chunk);
        }
    }
}

void ChunkManager::Draw(Renderer& renderer, const glm::mat4 viewProj, Shader& shader, Texture& tex)
{
    for (Chunk* chunk : m_InShotRenderList) {
        chunk->DrawSolid(renderer, viewProj, shader, tex);
    }
    for (Chunk* chunk : m_InShotRenderList) {
        chunk->DrawTransparent(renderer, viewProj, shader, tex);
    }
}

uint8_t ChunkManager::GetBlockAtPosition(const glm::vec3& position, const glm::ivec3& chunkPos)
{
    auto it = m_Chunks.find(chunkPos);
    if (it == m_Chunks.end() || it->second == nullptr)
        return 0;
    Chunk* chunk = it->second;
    return chunk->GetBlock(int(position.x), int(position.y), int(position.z));
}
