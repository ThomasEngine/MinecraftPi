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
       pair.second.reset(); 
   }  
   m_Chunks.clear(); 
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

    std::set<glm::ivec3, IVec3Less> neededChunks;
    for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; ++dx) {
        for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; ++dz) {
            glm::ivec3 pos = camChunk + glm::ivec3(dx, 0, dz);
            neededChunks.insert(pos);
        }
    }

    for (const auto& pos : neededChunks) {
        if (m_Chunks.find(pos) == m_Chunks.end() && m_ChunksScheduledForLoad.find(pos) == m_ChunksScheduledForLoad.end()) {
            m_ChunksToLoad.push(pos);
            m_ChunksScheduledForLoad.insert(pos);
        }
    }

    for (const auto& pair : m_Chunks) {
        if (neededChunks.find(pair.first) == neededChunks.end() && m_ChunksScheduledForUnload.find(pair.first) == m_ChunksScheduledForUnload.end()) {
            m_ChunksToUnload.push(pair.first);
            m_ChunksScheduledForUnload.insert(pair.first);
        }
    }
}

void ChunkManager::ProccessChunkLoadingAsync(Renderer& renderer)
{
    if (m_ChunkLoadTasks.empty()) {
        return;
    }

    for (auto& pair : m_ChunkLoadTasks) {
        ChunkLoadTask& task = pair.second;
        if (task.pendingSunlight && AreNeighborsLoaded(task.chunkPos)) {
            task.chunk->PropagateLight(*this);
            task.pendingSunlight = false;
            task.pendingSunlightFill = true;
        }
        else if (task.pendingSunlightFill && AreNeighborsLoaded(task.chunkPos)) {
            task.chunk->ApplySunlight(*this);
            task.pendingSunlightFill = false;
            task.pendingMesh = true;
        }
        else if (task.pendingMesh && AreNeighborsLoaded(task.chunkPos)) {
            task.chunk->createChunkMesh(renderer, *this);
            task.pendingMesh = false;
            m_ChunkLoadTasks.erase(pair.first);
            return;
        }
    }
}

void ChunkManager::ProcessChunkLoading(Renderer& renderer)  
{  
   if (m_ChunksToLoad.empty()) return;  
   glm::ivec3 pos = m_ChunksToLoad.front();  
   m_ChunksToLoad.pop();  
   m_ChunksScheduledForLoad.erase(pos);  

   std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(pos, *this);  
   m_Chunks[pos] = chunk;
   m_RenderList.push_back(chunk);  

   m_ChunkLoadTasks.emplace(pos, ChunkLoadTask{ pos, chunk });  
}




void ChunkManager::ProcessChunkUnloading(Renderer& renderer)
{
    if (m_ChunksToUnload.empty()) return;
    glm::ivec3 pos = m_ChunksToUnload.front();
    m_ChunksToUnload.pop();
    m_ChunksScheduledForUnload.erase(pos);
    auto it = m_Chunks.find(pos);
    if (it != m_Chunks.end()) {
        auto chunk = it->second;
        chunk->destroyMesh(renderer);
        m_Chunks.erase(it);
        m_ChunkLoadTasks.erase(pos);
        m_RenderList.erase(std::remove(m_RenderList.begin(), m_RenderList.end(), chunk), m_RenderList.end());
    }
}

void ChunkManager::UpdateInShotRenderList(const glm::mat4& viewProj)
{
    m_InShotRenderList.clear();
    frustum.Extract(viewProj);

    for (auto chunk : m_RenderList) {
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
    for (auto chunk : m_InShotRenderList) {
        chunk->DrawSolid(renderer, viewProj, shader, tex);
    }
    for (auto chunk : m_InShotRenderList) {
        chunk->DrawTransparent(renderer, viewProj, shader, tex);
    }
}

uint8_t ChunkManager::GetBlockAtPosition(const glm::vec3& position, const glm::ivec3& chunkPos)
{
    auto it = m_Chunks.find(chunkPos);
    if (it == m_Chunks.end() || it->second == nullptr)
        return 0;
    auto chunk = it->second;
    return chunk->GetBlock(int(position.x), int(position.y), int(position.z));
}

Chunk* ChunkManager::GetChunk(const glm::ivec3& chunkPos)  
{  
   auto it = m_Chunks.find(chunkPos);  
   if (it != m_Chunks.end()) {  
       return it->second.get();
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

    auto chunk = it->second;
    unsigned int index = x + y * CHUNK_SIZE_X + z * CHUNK_SIZE_X * CHUNK_SIZE_Y;
    uint8_t currentLight = chunk->GetLightLevel(index);
    if (lightLevel > currentLight) {
        chunk->SetLightLevel(index, lightLevel);
        chunk->sunlightBfsQueue.emplace(index);

		auto it = m_ChunkLoadTasks.find(chunkPos);
		if (it == m_ChunkLoadTasks.end()) {
			m_ChunkLoadTasks.emplace(chunkPos, ChunkLoadTask{ chunkPos, std::shared_ptr<Chunk>(chunk), true, false, false, false });
		}
		else {
			it->second.pendingSunlightFill = true;
		}
    }
}


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

