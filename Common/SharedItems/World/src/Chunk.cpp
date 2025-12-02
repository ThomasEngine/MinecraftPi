#include "World/include/Chunk.h"
#include <World/include/BlockRegistery.h>
#include <noise/FastNoiseLite.h>
#include "World/include/ChunkLoader.h"

static const FaceVertex faceVertices[6][4] = {
    // -Z (Back)
    { { {0,0,0}, {0,0} }, { {0,1,0}, {0,1} }, { {1,1,0}, {1,1} }, { {1,0,0}, {1,0} } },
    // +Z (Front)
    { { {0,0,1}, {0,0} }, { {1,0,1}, {1,0} }, { {1,1,1}, {1,1} }, { {0,1,1}, {0,1} } },
    // -Y (Bottom)
    { { {0,0,0}, {0,0} }, { {1,0,0}, {1,0} }, { {1,0,1}, {1,1} }, { {0,0,1}, {0,1} } },
    // +Y (Top)
    { { {0,1,0}, {0,0} }, { {0,1,1}, {0,1} }, { {1,1,1}, {1,1} }, { {1,1,0}, {1,0} } },
    // -X (Left)
    { { {0,0,0}, {0,0} }, { {0,0,1}, {1,0} }, { {0,1,1}, {1,1} }, { {0,1,0}, {0,1} } },
    // +X (Right)
    { { {1,0,0}, {0,0} }, { {1,1,0}, {0,1} }, { {1,1,1}, {1,1} }, { {1,0,1}, {1,0} } }
};

static const int faceDirs[6][3] = {
    {  0,  0, -1 }, // -Z (Back)
    {  0,  0,  1 }, // +Z (Front)
    {  0, -1,  0 }, // -Y (Bottom)
    {  0,  1,  0 }, // +Y (Top)
    { -1,  0,  0 }, // -X (Left)
    {  1,  0,  0 }  // +X (Right)
};


// https://www.redblobgames.com/maps/terrain-from-noise/
Chunk::Chunk(glm::ivec3 pos, ChunkLoader& owner)
	: chunkPos(pos)
{
	GenerateTerrain(owner);
}

Chunk::~Chunk()
{
}

void Chunk::SetBlock(int x, int y, int z, uint8_t type) {
    if (x < 0 || x >= CHUNK_SIZE_X ||
        y < 0 || y >= CHUNK_SIZE_Y ||
        z < 0 || z >= CHUNK_SIZE_Z)
        return;
    blocks[x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z)].blockID = type;
}

uint8_t Chunk::GetBlock(int x, int y, int z) const
{
    if (x < 0 || x >= CHUNK_SIZE_X ||
        y < 0 || y >= CHUNK_SIZE_Y ||
        z < 0 || z >= CHUNK_SIZE_Z)
        return 0;
    return blocks[x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z)].blockID;
}

Voxel Chunk::GetVoxel(int x, int y, int z) const
{
    int idx = GetBlockIndex(x, y, z);
    if (idx == -1) {
        return Voxel{ B_AIR, 0 };
    }
    return blocks[idx];
}

void Chunk::NeigbourVoxelQueue(int x, int y, int z, ChunkLoader& owner)
{
    glm::ivec3 pos = { x, y, z };
    for (int d = 0; d < 6; d++)
    {
		int nx = x + faceDirs[d][0];
		int ny = y + faceDirs[d][1];
		int nz = z + faceDirs[d][2];

		int neighborIdx = GetBlockIndex(nx, ny, nz);
        if (neighborIdx != -1)
        {
			sunlightBfsQueue.push(neighborIdx);
        }
    }
}

void Chunk::GenerateTerrain(ChunkLoader & owner)
{
    blocks.resize(CHUNKSIZE, { 0,0 });
    //memset(blocks, 0, sizeof(blocks));
    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
            // World coordinates
            float wx = float(chunkPos.x * CHUNK_SIZE_X + x);
            float wz = float(chunkPos.z * CHUNK_SIZE_Z + z);
            // Creating height
            float continental = owner.m_NoiseMaps->continentalness.GetNoise(wx, wz);
            float erosion = owner.m_NoiseMaps->erosion.GetNoise(wx, wz);
            float peaks = owner.m_NoiseMaps->peaksAndValleys.GetNoise(wx, wz);

            continental = (continental + 1.0f) * 0.5f;
            erosion = (erosion + 1.0f) * 0.5f;
            peaks = (peaks + 1.0f) * 0.5f;

            float baseHeight = 64.0f;
            float maxHeight = 164;

            float continentHeight = glm::clamp(glm::mix(0.0f, maxHeight, continental), 0.0f, maxHeight);
            float erosionEffect = glm::mix(1.0f, 0.5f, erosion);
            float peakEffect = glm::mix(0.0f, 32.0f, peaks);

            float height = continentHeight * erosionEffect + peakEffect;
            height = glm::clamp(height, 0.0f, float(CHUNK_SIZE_Y - 1));
            int intHeight =int(height);
            
            
            // Placing blocks
            for (int y = 0; y <= intHeight; ++y) {
                if (y == 0) {
                    SetBlock(x, y, z, B_BEDROCK);
                }
                else if (y == intHeight) {
                    if (height < baseHeight + 2)
                        SetBlock(x, y, z, B_SAND);
                    else if (height > maxHeight - 10)
                        SetBlock(x, y, z, B_GRAVEL);
                    else
                        SetBlock(x, y, z, B_GRASS);
                }
                else if (y > intHeight - 5) {
                    SetBlock(x, y, z, B_DIRT);
                }
                else {
                    SetBlock(x, y, z, B_STONE);
                }


            }

            for (int y = intHeight + 1; y <= baseHeight; ++y) {
                SetBlock(x, y, z, B_WATER);
            }
            for (int y = 1; y < intHeight; y++)
            {
                float caveNoise = owner.m_NoiseMaps->caveNoise.GetNoise((float)wx, (float)y, (float)wz);


                // carve out cave
                // Carving idea: https://www.youtube.com/watch?v=v7yyZZjF1z4
                if (caveNoise > -0.3f) {
                    int radius = 2; // Cave radius
                    for (int dx = -radius; dx <= radius; ++dx) {
                        for (int dy = -radius; dy <= radius; ++dy) {
                            for (int dz = -radius; dz <= radius; ++dz) {
                                // Spherical distance check
                                if (dx * dx + dy * dy + dz * dz <= radius * radius) {
                                    int nx = x + dx;
                                    int ny = y + dy;
                                    int nz = z + dz;
                                    SetBlock(nx, ny, nz, B_AIR);
                                }
                            }
                        }
                    }
                }
            }

        }
    }
}

//void Chunk::ApplySunlight(ChunkLoader& owner)
//{
//    while (!sunlightBfsQueue.empty()) {
//        unsigned int index = sunlightBfsQueue.front();
//        sunlightBfsQueue.pop();
//
//        int vx = index % CHUNK_SIZE_X;
//        int vy = (index / CHUNK_SIZE_X) % CHUNK_SIZE_Y;
//        int vz = index / (CHUNK_SIZE_X * CHUNK_SIZE_Y);
//
//        uint8_t lightLevel = blocks[index].lightLevel;
//        if (lightLevel <= 1) continue;
//
//        for (int d = 0; d < 6; ++d) {
//            int nx = vx + faceDirs[d][0];
//            int ny = vy + faceDirs[d][1];
//            int nz = vz + faceDirs[d][2];
//            int neighborIdx = GetBlockIndex(nx, ny, nz);
//            if (neighborIdx == -1) {
//                // Calculate world position of neighbor
//                glm::ivec3 chunkSize{ 16,0,16 };
//                glm::ivec3 worldPos = chunkPos * chunkSize + glm::ivec3(nx, ny, nz);
//                glm::ivec3 neighborChunkPos = owner.WorldToChunkPos(worldPos);
//                uint8_t newLight = (lightLevel > 0) ? lightLevel - 1 : 0;
//				uint8_t oldLight = owner.GetBlockLightLevel(worldPos);
//				if (oldLight >= newLight) continue;
//                owner.SetBlockLightLevel(worldPos, newLight);
//                continue;
//            }
//
//            Voxel& neighbor = blocks[neighborIdx];
//            if (!g_BlockTypes[neighbor.blockID].isTransparent) continue;
//            uint8_t newLight = (d == 2) ? lightLevel : (lightLevel > 0 ? lightLevel - 1 : 0); // d==4 is -Y (down)
//            if (neighbor.lightLevel >= newLight) continue;
//
//            neighbor.lightLevel = newLight;
//            sunlightBfsQueue.emplace(neighborIdx);
//        }
//    }
//}

void Chunk::ApplySunlight(ChunkLoader& owner)
{
    while (!sunlightBfsQueue.empty()) {
        unsigned int index = sunlightBfsQueue.front();
        sunlightBfsQueue.pop();

        Voxel& current = blocks[index];
        uint8_t lightLevel = current.lightLevel;
        if (lightLevel <= 1) continue;

        int vx = index % CHUNK_SIZE_X;
        int vy = (index / CHUNK_SIZE_X) % CHUNK_SIZE_Y;
        int vz = index / (CHUNK_SIZE_X * CHUNK_SIZE_Y);

        for (int d = 0; d < 6; ++d) {
            int nx = vx + faceDirs[d][0];
            int ny = vy + faceDirs[d][1];
            int nz = vz + faceDirs[d][2];
            int neighborIdx = GetBlockIndex(nx, ny, nz);

            uint8_t newLight = (d == 2) ? lightLevel : (lightLevel > 0 ? lightLevel - 1 : 0);

            if (neighborIdx == -1) {
                glm::ivec3 chunkSize{ 16,0,16 };
                glm::ivec3 worldPos = chunkPos * chunkSize + glm::ivec3(nx, ny, nz);
                uint8_t oldLight = owner.GetBlockLightLevel(worldPos);
                if (oldLight < newLight)
                    owner.SetBlockLightLevel(worldPos, newLight);
                continue;
            }

            Voxel& neighbor = blocks[neighborIdx];
            if (g_BlockTypes[neighbor.blockID].isTransparent && neighbor.lightLevel < newLight) {
                neighbor.lightLevel = newLight;
                sunlightBfsQueue.emplace(neighborIdx);
            }
        }
    }
}


void Chunk::PropagateLight(ChunkLoader& owner)
{
    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
            //int y = CHUNK_SIZE_Y - 1;
            int y = 150;
            int idx = GetBlockIndex(x, y, z);
			blocks[idx].lightLevel = 15;
            sunlightBfsQueue.push(idx);
        }
    }
}

    /*for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
            for (int y = CHUNK_SIZE_Y - 1; y >= 0; --y) {
                int idx = GetBlockIndex(x, y, z);
                if (idx == -1) break;
                Voxel& v = blocks[idx];
                if (g_BlockTypes[v.blockID].isTransparent) {
                    v.lightLevel = 15;
                }
                else {
                    int nIdx = GetBlockIndex(x, y + 1, z);
                    sunlightBfsQueue.push(nIdx);
                    break;
                }
            }
        }
    }*/
//}

void Chunk::ReapplyBorderLight(ChunkLoader& owner)
{
	for (int x = 0; x < CHUNK_SIZE_X; ++x) {
		for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
		    for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
				if (x == 0 || x == CHUNK_SIZE_X - 1 ||
					z == 0 || z == CHUNK_SIZE_Z - 1)
				{
                    int idx = GetBlockIndex(x, y, z);
					if (g_BlockTypes[blocks[idx].blockID].isTransparent) {
						sunlightBfsQueue.push(idx);
					}
                    else
                    {
                        break;
					}
				}
			}
		}
	}
}

inline int Chunk::GetBlockIndex(int x, int y, int z) const
{
    if (x < 0 || x >= CHUNK_SIZE_X ||
        y < 0 || y >= CHUNK_SIZE_Y ||
        z < 0 || z >= CHUNK_SIZE_Z)
        return -1;
    return x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z);
}

bool Chunk::IsEmpty(int x, int y, int z) const
{
    return GetBlock(x, y, z) != 0 ? true : false;
}


bool Chunk::NeighborIsEmpty(int nx, int ny, int nz, ChunkLoader& owner, int y) const
{
    if (ny < 0) return false;
    if (nx < 0 || nx >= CHUNK_SIZE_X ||
        nz < 0 || nz >= CHUNK_SIZE_Z)
    {
        glm::ivec3 neighborChunkPos = chunkPos;
        if (nx < 0) neighborChunkPos.x -= 1;
        else if (nx >= CHUNK_SIZE_X) neighborChunkPos.x += 1;
        if (nz < 0) neighborChunkPos.z -= 1;
        else if (nz >= CHUNK_SIZE_Z) neighborChunkPos.z += 1;
        neighborChunkPos.y = 0;


        glm::ivec3 neighborBlockPos = glm::ivec3(nx, ny, nz);
        neighborBlockPos.x = nx < 0 ? CHUNK_SIZE_X - 1 : (nx >= CHUNK_SIZE_X ? 0 : nx);
        neighborBlockPos.z = nz < 0 ? CHUNK_SIZE_Z - 1 : (nz >= CHUNK_SIZE_Z ? 0 : nz);
        uint8_t neighborChunkBlockId = owner.GetBlockAtPosition(neighborBlockPos, neighborChunkPos);
        if (neighborChunkBlockId != B_AIR && neighborChunkBlockId != B_WATER)
            return false;
    }
    else
    {
        uint8_t neighborBlockId = GetBlock(nx, ny, nz);
        if (neighborBlockId != B_AIR && neighborBlockId != B_WATER)
            return false;
    }
    return true;
}

float Chunk::FaceBrightness(FaceDirection face) const
{
    switch (face) {
    case FACE_TOP:      return 1.0f;  
    case FACE_FRONT:    return 0.8f;
    case FACE_BACK:     return 0.8f;
    case FACE_RIGHT:    return 0.6f;
    case FACE_LEFT:     return 0.6f;
    case FACE_BOTTOM:   return 0.5f; 
    }

}

void Chunk::destroyMesh(Renderer& ren)
{
	if (mesh) {
		ren.destroyMesh(*mesh);
	}
}

void Chunk::createChunkMesh(Renderer& renderer, ChunkLoader& owner)
{
	createSolidMesh(renderer, owner);
	if (hasTransparentBlocks) {
		createTransparentMesh(renderer, owner);
	}
}

void Chunk::createTransparentMesh(Renderer& renderer, ChunkLoader& owner)
{
    if (transparentMesh) {
        renderer.destroyMesh(*transparentMesh);
    }

    std::vector<FaceVertex> vertices;
    std::vector<unsigned int> indices;

    unsigned int indexOffset = 0;
	const int face = 3; // Top face only
    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                uint8_t blockId = GetBlock(x, y, z);
                if (blockId != B_WATER) continue;

                // Only draw the top layer of water
                if (blockId == B_WATER) {
                    uint8_t aboveBlock = (y + 1 < CHUNK_SIZE_Y) ? GetBlock(x, y + 1, z) : B_AIR;
                    if (aboveBlock == B_WATER) continue;
                }

                // Add 4 vertices for each face
                for (int v = 0; v < 4; ++v) {
                    const FaceVertex& fv = faceVertices[face][v];
                    glm::vec3 pos = fv.pos + glm::vec3(x, y, z) + glm::vec3(chunkPos.x * CHUNK_SIZE_X, chunkPos.y * CHUNK_SIZE_Y, chunkPos.z * CHUNK_SIZE_Z);
                    glm::vec2 tex = fv.tex;

                    const BlockType& blockType = g_BlockTypes[blockId];
                    uint8_t atlasIndex = blockType.textureIndices[face];

                    const int cols = 16;

                    float cellX = float(atlasIndex % cols);
                    float cellY = 15 - (atlasIndex / cols);

                    vertices.push_back(FaceVertex{ pos, tex, cellX, cellY, 1 });
                }
                // 6 indices for each face square
                indices.push_back(indexOffset + 0);
                indices.push_back(indexOffset + 1);
                indices.push_back(indexOffset + 2);
                indices.push_back(indexOffset + 0);
                indices.push_back(indexOffset + 2);
                indices.push_back(indexOffset + 3);
                indexOffset += 4;
            }
        }
    }
    transparentMesh = std::make_unique<Mesh>(renderer.uploadMesh(vertices, indices));
    vertices.clear();
    indices.clear();
}

void Chunk::createSolidMesh(Renderer& renderer, ChunkLoader& owner)
{
    if (mesh) {
        renderer.destroyMesh(*mesh);
    }

    std::vector<FaceVertex> vertices;
    std::vector<unsigned int> indices;

    unsigned int indexOffset = 0;

    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                uint8_t blockId = GetBlock(x, y, z);

                if (g_BlockTypes[blockId].isTransparent)
                {
                    if (blockId == B_WATER) hasTransparentBlocks = true;
                    continue;
                }

                for (int face = 0; face < 6; ++face) {
                    int nx = x + faceDirs[face][0];
                    int ny = y + faceDirs[face][1];
                    int nz = z + faceDirs[face][2];
					if (!NeighborIsEmpty(nx, ny, nz, owner, y)) continue;

                    // sample neighbor in face direction
                    int sx = x + faceDirs[face][0];
                    int sy = y + faceDirs[face][1];
                    int sz = z + faceDirs[face][2];
                    float sampledLight = 0.1f;
                    int sIdx = GetBlockIndex(sx, sy, sz);
                    if (sIdx != -1) {
                        sampledLight = float(blocks[sIdx].lightLevel) / 15.0f;
                    }
                    else {
                        
                        glm::ivec3 neighborChunkPos = chunkPos;
                        if (nx < 0) neighborChunkPos.x -= 1;
                        else if (nx >= CHUNK_SIZE_X) neighborChunkPos.x += 1;
                        if (nz < 0) neighborChunkPos.z -= 1;
                        else if (nz >= CHUNK_SIZE_Z) neighborChunkPos.z += 1;
                        neighborChunkPos.y = 0;


                        glm::ivec3 neighborBlockPos = glm::ivec3(nx, ny, nz);
                        neighborBlockPos.x = nx < 0 ? CHUNK_SIZE_X - 1 : (nx >= CHUNK_SIZE_X ? 0 : nx);
                        neighborBlockPos.z = nz < 0 ? CHUNK_SIZE_Z - 1 : (nz >= CHUNK_SIZE_Z ? 0 : nz);

                        const uint8_t& light = owner.GetLightAtPosition(neighborBlockPos, neighborChunkPos);
                        sampledLight = light / 15.0f;
                    }
                    float ambient = FaceBrightness((FaceDirection)face);
					sampledLight = glm::clamp(sampledLight, 0.1f, 1.0f);
                    float finalLight = sampledLight * ambient;


                    // Add 4 vertices for each face
                    for (int v = 0; v < 4; ++v) {
                        const FaceVertex& fv = faceVertices[face][v];

                        glm::vec3 pos = fv.pos + glm::vec3(x, y, z) + glm::vec3(chunkPos.x * CHUNK_SIZE_X, chunkPos.y * CHUNK_SIZE_Y, chunkPos.z * CHUNK_SIZE_Z);
                        glm::vec2 tex = fv.tex;

                        const BlockType& blockType = g_BlockTypes[blockId];
                        uint8_t atlasIndex = blockType.textureIndices[face];

                        const int cols = 16;

                        float cellX = float(atlasIndex % cols);
                        float cellY = 15 - (atlasIndex / cols);


                        vertices.push_back(FaceVertex{ pos, tex, cellX, cellY, finalLight });
                    }
                    // 6 indices for each face square
                    indices.push_back(indexOffset + 0);
                    indices.push_back(indexOffset + 1);
                    indices.push_back(indexOffset + 2);
                    indices.push_back(indexOffset + 0);
                    indices.push_back(indexOffset + 2);
                    indices.push_back(indexOffset + 3);
                    indexOffset += 4;
                }
            }
        }
    }
    mesh = std::make_unique<Mesh>(renderer.uploadMesh(vertices, indices));
    vertices.clear();
    indices.clear();
}

void Chunk::DrawSolid(Renderer& renderer, const glm::mat4& viewProj, const Shader& shader, const Texture& texture) const
{
    if (!mesh) return;
    renderer.drawMesh(*mesh, shader, viewProj, texture);
}

void Chunk::DrawTransparent(Renderer& renderer, const glm::mat4& viewProj, const Shader& shader, const Texture& texture) const
{
    if (hasTransparentBlocks) renderer.drawMesh(*transparentMesh, shader, viewProj, texture);
}

uint8_t Chunk::GetLightLevel(int x, int y, int z)
{
    int blockIndex = GetBlockIndex(x, y, z);
    if (blockIndex == -1) return 15;
    return blocks[blockIndex].lightLevel;
}

uint8_t Chunk::GetLightLevel(unsigned int index)
{
    return blocks[index].lightLevel;
}

void Chunk::SetLightLevel(int x, int y, int z, uint8_t newLight)
{
    if (newLight > 15) {
		blocks[GetBlockIndex(x, y, z)].lightLevel = 15;
		return;
    }
    blocks[GetBlockIndex(x, y, z)].lightLevel = newLight;
}

void Chunk::SetLightLevel(int index, uint8_t newLight)
{
    if (newLight > 15)
    {
        blocks[index].lightLevel = 15;
        return;
    }
    blocks[index].lightLevel = newLight;
}
