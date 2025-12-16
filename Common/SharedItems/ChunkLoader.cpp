#include "ChunkLoader.h"
#include "Chunk.h"
#include "Frustum.h"
#include "Camera.h"
#include "BlockRegistery.h"
#include <algorithm>

const glm::ivec3 offsets[] = {
    { 1, 0, 0 }, { -1, 0, 0 },
    { 0, 0, 1 }, { 0, 0, -1 }
};

#ifdef WINDOWS_BUILD
    static const int numThreads = 8;
#else
	static const int numThreads = 4;
#endif

ChunkLoader::ChunkLoader(Renderer& rend, std::shared_ptr<NoiseMaps> noiseMaps, bool& isReady)
	: m_CameraPos(-1.0f), m_CameraDir(0.0f), m_Renderer(rend), m_NoiseMaps(noiseMaps), frustum()
{
	   m_WorldReady = &isReady;

       HALF_X = CHUNK_SIZE_X / 2;
       HALF_Y = CHUNK_SIZE_Y / 2;
       HALF_Z = CHUNK_SIZE_Z / 2;
}


ChunkLoader::~ChunkLoader()  
{  
   for (auto& pair : m_ChunkLoadTasks) {  
       pair.second.chunk.reset(); 
   }  
   m_ChunkLoadTasks.clear();
}

glm::ivec3 ChunkLoader::WorldToChunkPos(const glm::vec3& pos) const
{
    return glm::ivec3(
        int(std::floor(pos.x / CHUNK_SIZE_X)),
        0,
        int(std::floor(pos.z / CHUNK_SIZE_Z))
    );
}

void ChunkLoader::SetBlockAtPosition(const glm::vec3& worldPos, const uint8_t& block)
{
	glm::ivec3 chunkPos = WorldToChunkPos(worldPos);
	glm::vec3 position = glm::vec3(
		int(std::floor(worldPos.x)) - chunkPos.x * CHUNK_SIZE_X,
		int(std::floor(worldPos.y)),
		int(std::floor(worldPos.z)) - chunkPos.z * CHUNK_SIZE_Z
	);
	auto it = m_ChunkLoadTasks.find(chunkPos);
	if (it != m_ChunkLoadTasks.end()) {
		it->second.chunk->SetBlock(int(position.x), int(position.y), int(position.z), block);
        it->second.chunk->NeigbourVoxelQueue(int(position.x), int(position.y), int(position.z), *this);
        ReloadNeighborChunks(chunkPos);
		it->second.pendingSunlightFill = true;
		it->second.pendingMesh = false;
		it->second.renderReady = false;
		it->second.reloaded = false;
	}
}

void ChunkLoader::RemoveBlockAtPosition(const glm::vec3& worldPos)
{
	glm::ivec3 chunkPos = WorldToChunkPos(worldPos);
	glm::vec3 position = glm::vec3(
		int(std::floor(worldPos.x)) - chunkPos.x * CHUNK_SIZE_X,
		int(std::floor(worldPos.y)),
		int(std::floor(worldPos.z)) - chunkPos.z * CHUNK_SIZE_Z
	);
	auto it = m_ChunkLoadTasks.find(chunkPos);
	if (it != m_ChunkLoadTasks.end()) {
		it->second.chunk->SetBlock(int(position.x), int(position.y), int(position.z), B_AIR);
		it->second.chunk->NeigbourVoxelQueue(int(position.x), int(position.y), int(position.z), *this);
		ReloadNeighborChunks(chunkPos);
		it->second.pendingSunlightFill = true;
		it->second.pendingMesh = false;
		it->second.renderReady = false;
		it->second.reloaded = false;
	}
}

void ChunkLoader::AddToSunlightQueue(const glm::ivec3& worldPos)
{
	glm::ivec3 chunkPos = WorldToChunkPos(glm::vec3(worldPos));
	glm::vec3 position = glm::vec3(
		int(std::floor(worldPos.x)) - chunkPos.x * CHUNK_SIZE_X,
		int(std::floor(worldPos.y)),
		int(std::floor(worldPos.z)) - chunkPos.z * CHUNK_SIZE_Z
	);
	auto it = m_ChunkLoadTasks.find(chunkPos);
	if (it != m_ChunkLoadTasks.end()) {
        int idx = position.x + CHUNK_SIZE_X * (position.y + CHUNK_SIZE_Y * position.z);
		it->second.chunk->sunlightBfsQueue.push(idx);
	}
}

void ChunkLoader::Update(const glm::vec3& camDir, const glm::vec3& camPos, const glm::mat4& viewProjMatrix)
{
    m_CameraDir = camDir;
    if (WorldToChunkPos(m_CameraPos) != WorldToChunkPos(camPos))
    {
        m_CameraPos = camPos;
        FindChunksToLoadAndUnload(camPos);
    }
    else
    {
		m_CameraPos = camPos;
    }
	bool ChunksToLoadEmpty = m_ChunksToLoad.empty();
    if (!ChunksToLoadEmpty) {
        ProcessChunkLoading(m_Renderer);
    } 
    if (!m_ChunksToUnload.empty()) {
        ProcessChunkUnloading(m_Renderer);
    }
    else if (ChunksToLoadEmpty) {
		ProccessChunkLoadingAsync(m_Renderer);
    }
    UpdateInShotRenderList(viewProjMatrix);
}

bool ChunkLoader::AreNeighborsLoaded(const glm::ivec3& pos) const {

    for (const auto& off : offsets) {
		auto it = m_ChunkLoadTasks.find(pos + off);
        if (m_ChunkLoadTasks.find(pos + off) == m_ChunkLoadTasks.end())
            return false;
    }
    return true;
}


void ChunkLoader::FindChunksToLoadAndUnload(const glm::vec3& camPos)
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
        if (m_ChunkLoadTasks.find(pos) == m_ChunkLoadTasks.end() && m_ChunksScheduledForLoad.find(pos) == m_ChunksScheduledForLoad.end()) {
            m_ChunksToLoad.push(pos);
			//m_AsyncChunkLoadQueue.push(pos);
            m_ChunksScheduledForLoad.insert(pos);
        }
    }

    for (const auto& pair : m_ChunkLoadTasks) {
        if (neededChunks.find(pair.first) == neededChunks.end() && m_ChunksScheduledForUnload.find(pair.first) == m_ChunksScheduledForUnload.end()) {
            m_ChunksToUnload.push(pair.first);
            m_ChunksScheduledForUnload.insert(pair.first);
        }
    }
}

void ChunkLoader::ProccessChunkLoadingAsync(Renderer& renderer)
{

//#pragma omp parallel for
    for (auto& pair : m_ChunkLoadTasks) {
        ChunkLoadTask& task = pair.second;
        if (task.pendingSunlightFill && AreNeighborsLoaded(task.chunkPos) && !task.reloaded) {
            //task.chunk->PropagateLight(*this);
            task.pendingSunlight = false;
            //task.pendingSunlightFill = true;
            task.chunk->ApplySunlight(*this);
            task.pendingSunlightFill = false;
            task.pendingMesh = true;
			if (!*m_WorldReady )
				*m_WorldReady = true;
        }
        //else if (task.pendingSunlightFill && AreNeighborsLoaded(task.chunkPos) && !task.reloaded) {
        //    task.chunk->ApplySunlight(*this);
        //    task.pendingSunlightFill = false;
        //    task.pendingMesh = true;
        //}
        else if (task.pendingMesh && AreNeighborsLoaded(task.chunkPos) && !task.reloaded) {
            task.chunk->createChunkMesh(renderer, *this);
            task.pendingMesh = false;
            task.reloaded = true;
			task.renderReady = true;
            return;
        }
    }
}

void ChunkLoader::ProcessChunkLoading(Renderer& renderer)
{
    if (m_ChunksToLoad.empty()) return;

    std::vector<glm::ivec3> chunkPositions;
    for (int i = 0; i < numThreads && !m_ChunksToLoad.empty(); ++i) {
        chunkPositions.push_back(m_ChunksToLoad.front());
        m_ChunksScheduledForLoad.erase(m_ChunksToLoad.front());
        m_ChunksToLoad.pop();
    }

    std::vector<std::shared_ptr<Chunk>> createdChunks(chunkPositions.size());

    #pragma omp parallel for
    for (int i = 0; i < int(chunkPositions.size()); ++i) {
        createdChunks[i] = std::make_shared<Chunk>(chunkPositions[i], *this);
    }

    for (int i = 0; i < static_cast<int>(chunkPositions.size()); ++i) {
        m_RenderList.push_back(createdChunks[i]);
        m_ChunkLoadTasks.emplace(chunkPositions[i], ChunkLoadTask{ chunkPositions[i], createdChunks[i] });
    }
}

void ChunkLoader::ProcessChunkUnloading(Renderer& renderer)
{
   if (m_ChunksToUnload.empty()) return;
   glm::ivec3 pos = m_ChunksToUnload.front();
   m_ChunksToUnload.pop();
   m_ChunksScheduledForUnload.erase(pos);
   auto it = m_ChunkLoadTasks.find(pos);
   if (it != m_ChunkLoadTasks.end()) {
       auto chunk = it->second.chunk;
       chunk->destroyMesh(renderer);
       m_ChunkLoadTasks.erase(pos);

       auto renderListIt = std::remove_if(
           m_RenderList.begin(),
           m_RenderList.end(),
           [&chunk](const std::shared_ptr<Chunk>& c) { return c == chunk; }
       );
       m_RenderList.erase(renderListIt, m_RenderList.end());
   }
}

void ChunkLoader::UpdateInShotRenderList(const glm::mat4& viewProj)
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

void ChunkLoader::ReloadNeighborChunks(const glm::ivec3& chunkPos)
{
	for (const auto& off : offsets) {
		glm::ivec3 neighborPos = chunkPos + off;
		auto it = m_ChunkLoadTasks.find(neighborPos);
		if (it != m_ChunkLoadTasks.end() && it->second.reloaded) {
			ChunkLoadTask& task = it->second;
			task.pendingSunlightFill = true;
			task.reloaded = false;
		}
	}
}

void ChunkLoader::Draw(const glm::mat4 viewProj, Shader& shader, Texture& tex)
{
    for (auto chunk : m_InShotRenderList) {
        chunk->DrawSolid(m_Renderer, viewProj, shader, tex);
    }
    for (auto chunk : m_InShotRenderList) {
        chunk->DrawTransparent(m_Renderer, viewProj, shader, tex);
    }
}

uint8_t ChunkLoader::GetBlockAtPosition(const glm::vec3& position, const glm::ivec3& chunkPos)
{
    auto it = m_ChunkLoadTasks.find(chunkPos);
    if (it == m_ChunkLoadTasks.end() || it->second.chunk == nullptr)
        return 0;
    auto chunk = it->second.chunk;
    return chunk->GetBlock(int(position.x), int(position.y), int(position.z));
}

uint8_t ChunkLoader::GetLightAtPosition(const glm::vec3& position, const glm::ivec3& chunkPos)
{
    auto it = m_ChunkLoadTasks.find(chunkPos);
    if (it == m_ChunkLoadTasks.end() || it->second.chunk == nullptr)
        return 0;
    auto chunk = it->second.chunk;
    return chunk->GetLightLevel(int(position.x), int(position.y), int(position.z));
}

Chunk* ChunkLoader::GetChunk(const glm::ivec3& chunkPos)  
{  
   auto it = m_ChunkLoadTasks.find(chunkPos);
   if (it != m_ChunkLoadTasks.end()) {
       return it->second.chunk.get();
   }  
   return nullptr;  
}

void ChunkLoader::SetBlockLightLevel(const glm::ivec3& worldPos, uint8_t lightLevel)
{
    glm::ivec3 chunkPos(
        int(std::floor(worldPos.x / CHUNK_SIZE_X)),
        0,
        int(std::floor(worldPos.z / CHUNK_SIZE_Z))
    );
	auto it = m_ChunkLoadTasks.find(chunkPos);
    if (it == m_ChunkLoadTasks.end() || it->second.chunk.get() == nullptr)
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

	auto chunk = it->second.chunk.get();
    unsigned int index = x + y * CHUNK_SIZE_X + z * CHUNK_SIZE_X * CHUNK_SIZE_Y;
    uint8_t currentLight = chunk->GetLightLevel(index);
    if (lightLevel >= currentLight) {
        chunk->SetLightLevel(index, lightLevel);
        chunk->sunlightBfsQueue.emplace(index);
		it->second.pendingSunlightFill = true;
    }
}


uint8_t ChunkLoader::GetBlockLightLevel(const glm::ivec3& worldPos)
{
    glm::ivec3 chunkPos(
        int(std::floor(worldPos.x / CHUNK_SIZE_X)),
        0,
        int(std::floor(worldPos.z / CHUNK_SIZE_Z))
    );

    auto it = m_ChunkLoadTasks.find(chunkPos);
    if (it == m_ChunkLoadTasks.end() || it->second.chunk == nullptr)
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

    return it->second.chunk->GetLightLevel(x, y, z);
}