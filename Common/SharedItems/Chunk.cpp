#include "Chunk.h"
#include <BlockRegistery.h>
#include <../noise/FastNoiseLite.h>
#include "ChunkLoader.h"
#include "FileData.h"

const FaceVertex faceVertices[6][4] = {
    // -Z (Back)
    { { {0,0,0}, {0,0} }, 
    { {0,1,0}, {0,1} }, 
    { {1,1,0}, {1,1} }, 
    { {1,0,0}, {1,0} } },
    // +Z (Front)
    { { {0,0,1}, {0,0} }, 
    { {1,0,1}, {1,0} }, 
    { {1,1,1}, {1,1} }, 
    { {0,1,1}, {0,1} } },
    // -Y (Bottom)
    { { {0,0,0}, {0,0} }, 
    { {1,0,0}, {1,0} }, 
    { {1,0,1}, {1,1} }, 
    { {0,0,1}, {0,1} } },
    // +Y (Top)
    { { {0,1,0}, {0,0} }, 
    { {0,1,1}, {0,1} }, 
    { {1,1,1}, {1,1} }, 
    { {1,1,0}, {1,0} } },
    // -X (Left)
    { { {0,0,0}, {0,0} }, 
    { {0,0,1}, {1,0} }, 
    { {0,1,1}, {1,1} }, 
    { {0,1,0}, {0,1} } },
    // +X (Right)
    { { {1,0,0}, {0,0} }, 
    { {1,1,0}, {0,1} }, 
    { {1,1,1}, {1,1} },
    { {1,0,1}, {1,0} } }
};

const int faceDirs[6][3] = {
    {  0,  0, -1 }, // -Z (Back)
    {  0,  0,  1 }, // +Z (Front)
    {  0, -1,  0 }, // -Y (Bottom)
    {  0,  1,  0 }, // +Y (Top)
    { -1,  0,  0 }, // -X (Left)
    {  1,  0,  0 }  // +X (Right)
};

const int aoOffsets[6][4][3][3] = {
    // -Z (Back)
	{ // side 1, side 2, corner
        { {-1,0,0}, {0,-1,0}, {-1,-1,0} }, // bottom-left
        { {-1,0,0}, {0,1,0}, {-1,1,0} },   // top-left
        { {1,0,0}, {0,1,0}, {1,1,0} },      // top-right
        { {1,0,0}, {0,-1,0}, {1,-1,0} },   // bottom-right
    },
    // +Z (Front)
    {
        { {-1,0,0}, {0,-1,0}, {-1,-1,0} }, // bottom-left
        { {1,0,0}, {0,-1,0}, {1,-1,0} },   // bottom-right
        { {1,0,0}, {0,1,0}, {1,1,0} },      // top-right
        { {-1,0,0}, {0,1,0}, {-1,1,0} },   // top-left
    },
    // -Y (Bottom)
    {
        { {-1,0,0}, {0,0,-1}, {-1,0,-1} }, // bottom-left
        { {1,0,0}, {0,0,-1}, {1,0,-1} },   // bottom-right
        { {-1,0,0}, {0,0,1}, {-1,0,1} },   // top-left
        { {1,0,0}, {0,0,1}, {1,0,1} },      // top-right
    },
    // +Y (Top)
    {
        { {-1,0,0}, {0,0,-1}, {-1,0,-1} }, // bottom-left
        { {-1,0,0}, {0,0,1}, {-1,0,1} },   // top-left
        { {1,0,0}, {0,0,1}, {1,0,1} },      // top-right
        { {1,0,0}, {0,0,-1}, {1,0,-1} },   // bottom-right
    },
    // -X (Left)
    {
        { {0,-1,0}, {0,0,-1}, {0,-1,-1} }, // bottom-left
        { {0,-1,0}, {0,0,1}, {0,-1,1} },   // top-left
        { {0,1,0}, {0,0,1}, {0,1,1} },      // top-right
        { {0,1,0}, {0,0,-1}, {0,1,-1} },   // bottom-right
    },
    // +X (Right)
    {
        { {0,0,0}, {0,0,-1}, {0,-1,-1} }, // bottom-left
        { {0,0,0}, {0,0,-1}, {0,1,-1} },   // bottom-right
        { {0,0,0}, {0,0,1}, {0,1,1} },      // top-right
        { {0,0,0}, {0,0,1}, {0,-1,1} },   // top-left
    }
};


namespace {
    // 0fps ambient occlusion logic
    float CalculateAO(bool side1, bool side2, bool corner) {
		float AOs[4] = { 0.45, 0.65, 0.8, 1.0 };
        if (side1 && side2) {
            return AOs[0];
        }
		int aoIndex = side1 + side2 + corner;
		return AOs[3 - aoIndex];
    }

    float TreeRandom(int x, int z, int seed = 1337)
    {
        int n = x + z * 57 + seed;
        n = (n << 13) ^ n;
        int nn = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
        return float(nn) / 2147483647.0f; // [0,1]
    }

    glm::ivec3 GetWorldPos(int dx, int dy, int dz, glm::ivec3 chunkPos)
    {
        glm::ivec3 worldPos = chunkPos * glm::ivec3(CHUNK_SIZE_X, 0, CHUNK_SIZE_Z) + glm::ivec3(dx, dy, dz);
		return worldPos;
    }
}



// https://www.redblobgames.com/maps/terrain-from-noise/
Chunk::Chunk(glm::ivec3 pos, ChunkLoader& owner, const std::vector<BlockData>& importData)
	: chunkPos(pos), mesh(nullptr), transparentMesh(nullptr)
{
	GenerateTerrain(owner, importData);
	PropagateLight(owner);
	m_FinishedGeneration = true;
}

Chunk::~Chunk()
{
}

void Chunk::SetBlock(int x, int y, int z, uint8_t type) {
    if (x < 0 || x >= CHUNK_SIZE_X ||
        y < 0 || y >= CHUNK_SIZE_Y ||
        z < 0 || z >= CHUNK_SIZE_Z)
        return;
	unsigned int index = x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z);
    blocks[index].blockID = type;
    BlockData data;
    data.id = type;
    data.index = index;
    if (m_FinishedGeneration) changedBlocks.push_back(data);
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
        if (neighborIdx == -1) // other chunk
        {
			glm::ivec3 chunkSize{ 16,0,16 };

            glm::ivec3 neighborChunkPos = chunkPos;
            if (nx < 0) neighborChunkPos.x -= 1;
            else if (nx >= CHUNK_SIZE_X) neighborChunkPos.x += 1;
            if (nz < 0) neighborChunkPos.z -= 1;
            else if (nz >= CHUNK_SIZE_Z) neighborChunkPos.z += 1;
            neighborChunkPos.y = 0;


            glm::ivec3 neighborBlockPos = glm::ivec3(nx, ny, nz);
            neighborBlockPos.x = nx < 0 ? CHUNK_SIZE_X - 1 : (nx >= CHUNK_SIZE_X ? 0 : nx);
            neighborBlockPos.z = nz < 0 ? CHUNK_SIZE_Z - 1 : (nz >= CHUNK_SIZE_Z ? 0 : nz);

			glm::ivec3 worldPos = neighborChunkPos * chunkSize + neighborBlockPos;
			owner.AddToSunlightQueue(worldPos);
            continue;
        }
        sunlightBfsQueue.push(neighborIdx);
    }
}

void Chunk::GenerateTerrain(ChunkLoader & owner, const std::vector<BlockData>& loadedData)
{
    blocks.resize(CHUNKSIZE, { 0,0 });

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
                    {
                        SetBlock(x, y, z, B_GRASS);
                        glm::ivec3 chunkSize = { 16, 0 ,16 };
						glm::ivec3 worldPos = chunkPos * chunkSize + glm::ivec3(x,y,z);
                        if (TreeRandom(worldPos.x, worldPos.z) >  0.99f)
                        {
                            worldPos.y += 1;
							m_TreePositions.push_back(worldPos);
                        }
                    }
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
	// Load imported data (is only the changed data)
    if (!loadedData.empty())
    {
        for (const auto& blockData : loadedData)
        {
            blocks[blockData.index].blockID = blockData.id;
			changedBlocks.push_back(blockData); // restore changed blocks
        }
    }
}

void Chunk::PlaceTrees(Renderer& ren, ChunkLoader& owner)
{
    if (!m_TreePositions.empty())
    {
        for (const auto& worldPos : m_TreePositions)
        {
            if (CanPlaceTreeAt(worldPos))
                owner.PlaceTree(worldPos);
        }
		m_TreePositions.clear();
		PropagateLight(owner);
    }

}

void Chunk::ExportChangedBlocks(FileData& fileHelper)
{
	// Export only changed blocks
	fileHelper.SaveChunkData(chunkPos.x, chunkPos.y, chunkPos.z, changedBlocks);
}

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
                {
                    owner.SetBlockLightLevel(worldPos, newLight);
                }
                continue;
            }

            Voxel& neighbor = blocks[neighborIdx];
            if (neighbor.blockID == B_WATER)
            {
				newLight = newLight > 4 ? newLight - 2 : 0;
            }

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
        if (!g_BlockTypes[neighborChunkBlockId].isTransparent)
            return false;
    }
    else
    {
        uint8_t neighborBlockId = GetBlock(nx, ny, nz);
        if (!g_BlockTypes[neighborBlockId].isTransparent)
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

AONeighbors Chunk::GetAONeighbors(int face, int vertex, int x, int y, int z, ChunkLoader& owner, int localY) const
{
    AONeighbors result;

    auto check = [&](int dx, int dy, int dz) {
        return !NeighborIsEmpty(x + dx, y + dy, z + dz, owner, localY);
        };

    result.side1 = check(aoOffsets[face][vertex][0][0],
        aoOffsets[face][vertex][0][1],
        aoOffsets[face][vertex][0][2]);

    result.side2 = check(aoOffsets[face][vertex][1][0],
        aoOffsets[face][vertex][1][1],
        aoOffsets[face][vertex][1][2]);

    result.corner = check(aoOffsets[face][vertex][2][0],
        aoOffsets[face][vertex][2][1],
        aoOffsets[face][vertex][2][2]);

    return result;
}

bool Chunk::CanPlaceTreeAt(const glm::ivec3& worldPos) const
{
    glm::ivec3 location = worldPos;
	// worldPos to chunk local pos
	glm::ivec3 chunkSize = { CHUNK_SIZE_X, 0 ,CHUNK_SIZE_Z };
	location -= chunkPos * chunkSize;

	// check block below is dirt or grass
    location.y -= 1;

	int index = GetBlockIndex(location.x, location.y, location.z);
	if (index == -1) return false;
	uint8_t blockId = blocks[index].blockID;
	if (blockId != B_GRASS && blockId != B_DIRT) return false;
	return true;
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

void Chunk::uploadChunkMesh(Renderer& renderer, ChunkLoader& owner)
{
    if (transparentMesh) renderer.uploadMesh(*transparentMesh);
    if (mesh) renderer.uploadMesh(*mesh);

}

void Chunk::createTransparentMesh(Renderer& renderer, ChunkLoader& owner)
{
    if (transparentMesh) {
        renderer.destroyMesh(*transparentMesh);
    }

    std::vector<FaceVertex> vertices;
	vertices.reserve(2000);
    std::vector<unsigned int> indices;

    unsigned int indexOffset = 0;
	const int face = 3; // Top face only
    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                uint8_t blockId = GetBlock(x, y, z);
                if (!g_BlockTypes[blockId].isTransparent || blockId == B_AIR) continue;

                // Only draw the top layer of water
                if (blockId == B_WATER) {
                    uint8_t aboveBlock = (y + 1 < CHUNK_SIZE_Y) ? GetBlock(x, y + 1, z) : B_AIR;
                    if (aboveBlock == B_WATER) continue;

                    // Add 4 vertices for each face
                    for (int v = 0; v < 4; ++v) {
                        const FaceVertex& fv = faceVertices[face][v];
                        glm::vec3 pos = fv.pos + glm::vec3(x, y, z) + glm::vec3(chunkPos.x * CHUNK_SIZE_X, chunkPos.y * CHUNK_SIZE_Y, chunkPos.z * CHUNK_SIZE_Z);
                        glm::vec2 tex = fv.tex;

                        const BlockType& blockType = g_BlockTypes[blockId];
                        AtlasPos atlasIndex = blockType.textureIndices[face];

                        vertices.push_back(FaceVertex{ pos, tex, (float)atlasIndex.x, 31 - (float)atlasIndex.y, 1 });
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
                else // not water
                {
                    for (int face = 0; face < 6; ++face) {
						int nx = x + faceDirs[face][0];
						int ny = y + faceDirs[face][1];
						int nz = z + faceDirs[face][2];

                        // Add 4 vertices for each face
                        for (int v = 0; v < 4; ++v) {
                            const FaceVertex& fv = faceVertices[face][v];

                            glm::vec3 pos = fv.pos + glm::vec3(x, y, z)
                                + glm::vec3(chunkPos.x * CHUNK_SIZE_X,
                                    chunkPos.y * CHUNK_SIZE_Y,
                                    chunkPos.z * CHUNK_SIZE_Z);

                            glm::vec2 tex = fv.tex;

                            // AO neighbors
                            AONeighbors aoN = GetAONeighbors(face, v, nx, ny, nz, owner, y);
                            float ao = CalculateAO(aoN.side1, aoN.side2, aoN.corner);

                            const BlockType& blockType = g_BlockTypes[blockId];
                            AtlasPos atlasIndex = blockType.textureIndices[face];

                            vertices.push_back(FaceVertex{ pos, tex, (float)atlasIndex.x, 31 - (float)atlasIndex.y, 1, ao});
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
    }
    transparentMesh = std::make_unique<Mesh>(vertices, indices);
	hasTransparentBlocks = true;
    renderer.uploadMesh(*transparentMesh);
    vertices.clear();
	transparentMesh.get()->vertices.clear();
	transparentMesh.get()->vertices.shrink_to_fit();
	transparentMesh.get()->indices.clear();
	transparentMesh.get()->indices.shrink_to_fit();
    indices.clear();
}

void Chunk::createSolidMesh(Renderer& renderer, ChunkLoader& owner)
{
    if (mesh) {
        renderer.destroyMesh(*mesh);
    }
    std::vector<FaceVertex> vertices;
    vertices.reserve(4000);
    std::vector<unsigned int> indices;

    unsigned int indexOffset = 0;

    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                uint8_t blockId = GetBlock(x, y, z);

                if (g_BlockTypes[blockId].isTransparent)
                {
                    hasTransparentBlocks = true;
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
                    //float ambient = FaceBrightness((FaceDirection)face);
					sampledLight = glm::clamp(sampledLight, 0.1f, 1.0f);
                    float finalLight = sampledLight;


                    // Add 4 vertices for each face
                    for (int v = 0; v < 4; ++v) {
                        const FaceVertex& fv = faceVertices[face][v];

                        glm::vec3 pos = fv.pos + glm::vec3(x, y, z)
                            + glm::vec3(chunkPos.x * CHUNK_SIZE_X,
                                chunkPos.y * CHUNK_SIZE_Y,
                                chunkPos.z * CHUNK_SIZE_Z);

                        glm::vec2 tex = fv.tex;

                        // AO neighbors
                        AONeighbors aoN = GetAONeighbors(face, v, nx, ny , nz, owner, y);
                        float ao = CalculateAO(aoN.side1, aoN.side2, aoN.corner);

                        const BlockType& blockType = g_BlockTypes[blockId];
                        AtlasPos atlasIndex = blockType.textureIndices[face];
                        
                        vertices.push_back(FaceVertex{ pos, tex, (float)atlasIndex.x, 31 - (float)atlasIndex.y, finalLight, ao });
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

    mesh = std::make_unique<Mesh>(vertices, indices);
    renderer.uploadMesh(*mesh);
    vertices.clear();
    indices.clear();
	mesh.get()->vertices.clear();
	mesh.get()->vertices.shrink_to_fit();
	mesh.get()->indices.clear();
	mesh.get()->indices.shrink_to_fit();
}

void Chunk::DrawSolid(Renderer& renderer, const glm::mat4& viewProj, const Shader& shader, const Texture& texture) const
{
    if (!mesh) return;
	renderer.drawBatchMesh(*mesh);
}

void Chunk::DrawTransparent(Renderer& renderer, const glm::mat4& viewProj, const Shader& shader, const Texture& texture) const
{
    if (hasTransparentBlocks) renderer.drawBatchMesh(*transparentMesh);
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
