#include "World/include/ChunkManager.h"
#include "World/include/Chunk.h"
#include <cmath>
#include "Camera/include/Frustum.h"
#include "Camera/include/Camera.h"

const glm::ivec3 offsets[] = {
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
    m_PeaksAndValleys.SetFrequency(0.005f);
    m_PeaksAndValleys.SetFractalType(FastNoiseLite::FractalType_Ridged);

    // Caves
	m_CaveNoise = FastNoiseLite(9001);
	m_CaveNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	m_CaveNoise.SetFrequency(0.02f);


    HALF_X = CHUNK_SIZE_X / 2;
    HALF_Y = CHUNK_SIZE_Y / 2;
    HALF_Z = CHUNK_SIZE_Z / 2;
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
    else {
		ProccessChunkLoadingAsync(renderer);
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

void ChunkManager::ProccessChunkLoadingAsync(Renderer& renderer)
{
    if (!m_PendingSunlight.empty())
    {
        for (auto it = m_PendingSunlight.begin(); it != m_PendingSunlight.end(); ) {
            Chunk* chunk = *it;
            if (AreNeighborsLoaded(chunk->chunkPos)) {
                chunk->ApplyPropagatedLight(*this);

                it = m_PendingSunlight.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    if (!m_PendingSunlightFill.empty())
    {
        for (auto it = m_PendingSunlightFill.begin(); it != m_PendingSunlightFill.end(); ) {
            Chunk* chunk = *it;
            if(AreNeighborsLoaded(chunk->chunkPos)) {
                chunk->ApplySunlight(*this);

                // Border light propagation for all neighbors
                for (const auto& offset : offsets) {
                    glm::ivec3 neighborPos = chunk->chunkPos + offset;
                    auto itNeighbor = m_Chunks.find(neighborPos);
                    if (itNeighbor != m_Chunks.end() && itNeighbor->second) {
                        itNeighbor->second->ReapplyBorderLight(*this);
                        itNeighbor->second->ApplySunlight(*this);

                        if (std::find(m_PendingMesh.begin(), m_PendingMesh.end(), itNeighbor->second) == m_PendingMesh.end()) {
                            m_PendingMesh.push_back(itNeighbor->second);
                        }
                    }
                }

                it = m_PendingSunlightFill.erase(it);
            }
    else {
        ++it;
    }
        }
    }

    if (!m_PendingMesh.empty())
    {
        for (auto it = m_PendingMesh.begin(); it != m_PendingMesh.end(); ) {
            Chunk* chunk = *it;
            if (AreNeighborsLoaded(chunk->chunkPos)) {
                chunk->createChunkMesh(renderer, *this);
                it = m_PendingMesh.erase(it);
                break;
            }
            else {
                ++it;
            }
        }
    }
}

void ChunkManager::ProcessChunkLoading(Renderer& renderer)
{
    if (m_ChunksToLoad.empty()) return;
    glm::ivec3 pos = m_ChunksToLoad.front();
    m_ChunksToLoad.pop();
    m_ChunksScheduledForLoad.erase(pos);

    Chunk* chunk = new Chunk(pos, *this);
    m_Chunks[pos] = chunk;
    m_RenderList.push_back(chunk);

    m_PendingSunlight.push_back(chunk);
    m_PendingSunlightFill.push_back(chunk);
    m_PendingMesh.push_back(chunk);
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
    frustum.Extract(viewProj);

    for (Chunk* chunk : m_RenderList) {
        glm::vec3 center = glm::vec3(
            (chunk->chunkPos.x + 0.5f) * CHUNK_SIZE_X,
            CHUNK_SIZE_Y / 2.0f,
            (chunk->chunkPos.z + 0.5f) * CHUNK_SIZE_Z
        );

        if (frustum.BoxInFrustum(center, HALF_X, HALF_Y, HALF_Z)) {
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

uint8_t ChunkManager::GetBlockAtPosition(const glm::vec3& worldPos)
{
    return 0;
}

void ChunkManager::PlaceBlockAtPosition(const glm::vec3& worldPos)
{
}

Chunk* ChunkManager::GetChunk(const glm::ivec3& chunkPos)
{
	auto it = m_Chunks.find(chunkPos);
	if (it != m_Chunks.end()) {
		return it->second;
	}
	return nullptr;
}

void ChunkManager::SetBlockLightLevel(const glm::ivec3& worldPos, uint8_t lightLevel)
{
    glm::ivec3 chunkPos(
        int(std::floor(worldPos.x / CHUNK_SIZE_X)),
        0,
        int(std::floor(worldPos.z / CHUNK_SIZE_Z))
    );

    auto it = m_Chunks.find(chunkPos);
    if (it == m_Chunks.end() || it->second == nullptr)
        return;

    int x = worldPos.x % CHUNK_SIZE_X;
    int y = worldPos.y;
    int z = worldPos.z % CHUNK_SIZE_Z;
    if (x < 0) x += CHUNK_SIZE_X;
    if (z < 0) z += CHUNK_SIZE_Z;
    if (x < 0 || x >= CHUNK_SIZE_X ||
        y < 0 || y >= CHUNK_SIZE_Y ||
        z < 0 || z >= CHUNK_SIZE_Z)
        return;

    Chunk* chunk = it->second;
    unsigned int index = x + y * CHUNK_SIZE_X + z * CHUNK_SIZE_X * CHUNK_SIZE_Y;
    uint8_t currentLight = chunk->GetLightLevel(index);
    if (lightLevel > currentLight) {
        chunk->SetLightLevel(index, lightLevel);
        chunk->sunlightBfsQueue.emplace(index);
        if (std::find(m_PendingSunlightFill.begin(), m_PendingSunlightFill.end(), chunk) == m_PendingSunlightFill.end()) {
            m_PendingSunlightFill.push_back(chunk);
        }
        if (std::find(m_PendingMesh.begin(), m_PendingMesh.end(), chunk) == m_PendingMesh.end()) {
            m_PendingMesh.push_back(chunk);
        }
    }
}


//void ChunkManager::SetBlockLightLevel(const glm::ivec3& worldPos, uint8_t lightLevel)
//{
//    glm::ivec3 chunkPos(
//        int(std::floor(worldPos.x / CHUNK_SIZE_X)),
//        0,
//        int(std::floor(worldPos.z / CHUNK_SIZE_Z))
//    );
//
//    auto it = m_Chunks.find(chunkPos);
//    if (it == m_Chunks.end() || it->second == nullptr)
//        return;
//
//    int x = worldPos.x % CHUNK_SIZE_X;
//    int y = worldPos.y;
//    int z = worldPos.z % CHUNK_SIZE_Z;
//
//
//
//    if (x < 0) x += CHUNK_SIZE_X;
//    if (z < 0) z += CHUNK_SIZE_Z;
//
//    if (x < 0 || x >= CHUNK_SIZE_X ||
//        y < 0 || y >= CHUNK_SIZE_Y ||
//        z < 0 || z >= CHUNK_SIZE_Z)
//        return;
//
//    Chunk* chunk = it->second;
//    unsigned int index = x + y * CHUNK_SIZE_X + z * CHUNK_SIZE_X * CHUNK_SIZE_Y;
//    uint8_t currentLight = chunk->GetLightLevel(index);
//    if (lightLevel > currentLight) {
//        currentLight = lightLevel;
//        chunk->sunlightBfsQueue.emplace(index);
//    }
//}

uint8_t ChunkManager::GetBlockLightLevel(const glm::ivec3& worldPos)
{
    glm::ivec3 chunkPos(
        int(std::floor(worldPos.x / CHUNK_SIZE_X)),
        0,
        int(std::floor(worldPos.z / CHUNK_SIZE_Z))
    );

    auto it = m_Chunks.find(chunkPos);
    if (it == m_Chunks.end() || it->second == nullptr)
        return 0; 

    int x = worldPos.x % CHUNK_SIZE_X;
    int y = worldPos.y;
    int z = worldPos.z % CHUNK_SIZE_Z;

    if (x < 0) x += CHUNK_SIZE_X;
    if (z < 0) z += CHUNK_SIZE_Z;

    if (x < 0 || x >= CHUNK_SIZE_X ||
        y < 0 || y >= CHUNK_SIZE_Y ||
        z < 0 || z >= CHUNK_SIZE_Z)
        return 0; 

    return it->second->GetLightLevel(x, y, z);
}

