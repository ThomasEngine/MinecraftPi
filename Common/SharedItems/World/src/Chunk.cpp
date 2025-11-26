#include "World/include/Chunk.h"
#include <World/include/BlockRegistery.h>
#include <noise/FastNoiseLite.h>
#include "World/include/ChunkManager.h"

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
Chunk::Chunk(glm::ivec3 pos, ChunkManager& owner)
    : chunkPos(pos)
{
	GenerateTerrain(owner);
	ApplySunlight(owner);
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

void Chunk::GenerateTerrain(ChunkManager& owner)
{
    blocks.resize(CHUNKSIZE, { 0,0 });
    //memset(blocks, 0, sizeof(blocks));
    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
            // World coordinates
            float wx = float(chunkPos.x * CHUNK_SIZE_X + x);
            float wz = float(chunkPos.z * CHUNK_SIZE_Z + z);

            // Creating height
            float continental = owner.m_Continentalness.GetNoise(wx, wz);
            float erosion = owner.m_Erosion.GetNoise(wx, wz);
            float peaks = owner.m_PeaksAndValleys.GetNoise(wx, wz);

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



            // Debuging:
            // 
            if (x == 8 && z == 8) intHeight += 2;
            // 
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
                float caveNoise = owner.m_CaveNoise.GetNoise((float)wx, (float)y, (float)wz);


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

void Chunk::ApplySunlight(ChunkManager& owner)
{
    for(int x = 0; x < CHUNK_SIZE_X; ++x) {
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
    }
    int amount = 0;
    for (auto& v : blocks)
        if (v.lightLevel == 15) amount++;

    while (!sunlightBfsQueue.empty()) {
        unsigned int index = sunlightBfsQueue.front();
        sunlightBfsQueue.pop();

        int vx = index % CHUNK_SIZE_X;
        int vy = (index / CHUNK_SIZE_X) % CHUNK_SIZE_Y;
        int vz = index / (CHUNK_SIZE_X * CHUNK_SIZE_Y);

        uint8_t lightLevel = blocks[index].lightLevel;
        if (lightLevel <= 1) continue;

        for (int d = 0; d < 6; ++d) {
            int nx = vx + faceDirs[d][0];
            int ny = vy + faceDirs[d][1];
            int nz = vz + faceDirs[d][2];
            int neighborIdx = GetBlockIndex(nx, ny, nz);
            if (neighborIdx == -1) {
                // handle cross-chunk neighbor via owner if needed
                continue;
            }

            Voxel& neighbor = blocks[neighborIdx];
            if (!g_BlockTypes[neighbor.blockID].isTransparent) continue;

            // uniform attenuation: every step reduces light by 1
            uint8_t newLight = (lightLevel > 0) ? lightLevel - 1 : 0;
            if (neighbor.lightLevel >= newLight) continue;

            neighbor.lightLevel = newLight;
            sunlightBfsQueue.push(neighborIdx);
        }
    }
}


//void Chunk::ApplySunlight(ChunkManager& owner)
//{
//    //for (int x = 0; x < CHUNK_SIZE_X; ++x) {
//    //    for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
//    //        for (int y = CHUNK_SIZE_Y - 1; y >= 0; --y) {
//		  //      int idx = GetBlockIndex(x, y, z);
//	   //         Voxel& v = blocks[idx];
//    //            if (g_BlockTypes[v.blockID].isTransparent)
//    //            {
//    //                v.lightLevel = 15;
//    //            }
//    //            else
//    //            {
//    //                v.lightLevel = 14;
//			 //       break; 
//    //            }
//    //        }
//    //    }
//    //}
//
//
//
//    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
//        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
//            // Start from the top and go down
//            for (int y = CHUNK_SIZE_Y - 1; y >= 0; --y) {
//                int idx = GetBlockIndex(x, y, z);
//                Voxel& v = blocks[idx];
//                if (g_BlockTypes[v.blockID].isTransparent) {
//                    v.lightLevel = 15; // Set max sunlight
//                }
//                else {
//                    v.lightLevel = 15;
//                    break; // Stop at first solid block
//                }
//            }
//        }
//    }
//
//    // Now flood fill sunlight from all top-exposed air blocks
//    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
//        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
//            for (int y = CHUNK_SIZE_Y - 1; y >= 0; --y) {
//                int idx = GetBlockIndex(x, y, z);
//                Voxel& v = blocks[idx];
//                if (v.lightLevel == 15 &&
//                    g_BlockTypes[blocks[GetBlockIndex(x,y - 1,z)].blockID].isSolid == true) {
//                    FloodFillLight(x, y, z, owner);
//                }
//                else {
//                    break;
//                }
//            }
//        }
//    }
//
//
//        /*const int MAX_LIGHT = 15;
//        int currentLight = MAX_LIGHT;
//        for (int x = 0; x < CHUNK_SIZE_X; ++x) {
//            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
//                currentLight = MAX_LIGHT;
//                for (int y = CHUNK_SIZE_Y - 2; y >= 1; --y) {
//                    int index = GetBlockIndex(x, y ,z);
//                    int indexAbove = GetBlockIndex(x, y + 1, z);
//                    int indexFront = GetBlockIndex(x, y, z + 1);
//                    int indexBack = GetBlockIndex(x, y, z - 1);
//                    int indexLeft = GetBlockIndex(x - 1, y, z);
//                    int indexRight = GetBlockIndex(x + 1, y, z);
//                    Voxel& v = blocks[index];
//                    Voxel& vAbve = blocks[indexAbove];
//                    Voxel& vFront = blocks[indexFront];
//                    Voxel& vBack = blocks[indexBack];
//                    Voxel& vLeft = blocks[indexLeft];
//                    Voxel& vRight = blocks[indexRight];
//
//                    if (g_BlockTypes[vAbve.blockID].isTransparent) {
//                        v.lightLevel = currentLight;
//                    }
//                    else if (g_BlockTypes[vFront.blockID].isTransparent ||
//                        g_BlockTypes[vBack.blockID].isTransparent ||
//                        g_BlockTypes[vLeft.blockID].isTransparent ||
//                        g_BlockTypes[vRight.blockID].isTransparent) {
//                        if (currentLight - 1 < 3) {
//                            v.lightLevel = 3;
//                        }
//                        else {
//                            v.lightLevel = currentLight - 1;
//                        }
//                        v.lightLevel = currentLight;
//                    }
//                    else {
//                        v.lightLevel = 4;
//                        currentLight = 3;
//                    }
//                }
//            }
//        }*/
//}

int Chunk::GetBlockIndex(int x, int y, int z) const
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

bool Chunk::NeighborIsEmpty(int nx, int ny, int nz, ChunkManager& owner, int y) const
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

void Chunk::createChunkMesh(Renderer& renderer, ChunkManager& owner)
{
	createSolidMesh(renderer, owner);
	if (hasTransparentBlocks) {
		createTransparentMesh(renderer, owner);
	}
}

void Chunk::createTransparentMesh(Renderer& renderer, ChunkManager& owner)
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

void Chunk::createSolidMesh(Renderer& renderer, ChunkManager& owner)
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
                    float sampledLight = 0.0f;
                    int sIdx = GetBlockIndex(sx, sy, sz);
                    if (sIdx != -1) {
                        sampledLight = float(blocks[sIdx].lightLevel) / 15.0f;
                    }
                    else {
                        //check chunkManager for neighbor chunk
                        sampledLight = 0.0f;
                    }
                    float ambient = FaceBrightness((FaceDirection)face);
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

void Chunk::FloodFillLight(int x, int y, int z, ChunkManager& owner)
{
    struct Node { unsigned int index; };
    std::queue<Node> q;
    unsigned int BlockIndex = GetBlockIndex(x,y,z);
    if (BlockIndex == -1) return;
    Node n{ BlockIndex };
    q.emplace(n);

	while (!q.empty()) {
		unsigned int index = q.front().index;
		q.pop();
		int vx = index % CHUNK_SIZE_X;
		int vy = (index / (CHUNK_SIZE_X * CHUNK_SIZE_Z));
		int vz = (index / CHUNK_SIZE_X) % CHUNK_SIZE_Z;
		if (vx < 0 || vx >= CHUNK_SIZE_X ||
			vy < 0 || vy >= CHUNK_SIZE_Y ||
			vz < 0 || vz >= CHUNK_SIZE_Z)
			continue; 
		
        uint8_t lightLevel = GetLightLevel(vx, vy, vz);

        for (int d = 0; d < 6; d++)
        {
            unsigned int neighbourIndex = GetBlockIndex(
                vx + faceDirs[d][0], 
                vy + faceDirs[d][1], 
                vz + faceDirs[d][2]);
            
            if (neighbourIndex == -1)
            {
                // Check neighbouring chunks
                continue;

            }
            

            if (!g_BlockTypes[blocks[neighbourIndex].blockID].isTransparent
                && GetLightLevel(neighbourIndex) + 2 <= lightLevel)
            {
                SetLightLevel(neighbourIndex, lightLevel - 1);
                q.emplace(Node{ neighbourIndex });
            }
        }
	}
}

void Chunk::FloodFillLight(int x, int y, int z, uint8_t lightLevel, ChunkManager& owner)
{
 //   struct Node { int index; uint8_t light; };
 //   std::queue<Node> q;
	//int BlockIndex = GetBlockIndex(x, y, z);
	//if (BlockIndex ==  -1) return;
 //   q.push({ GetBlockIndex(x, y, z), lightLevel});

 //   while (!q.empty()) {
 //       Node n = q.front(); q.pop();

 //       int vx = n.index % CHUNK_SIZE_X;
 //       int vy = (n.index / (CHUNK_SIZE_X * CHUNK_SIZE_Z));
 //       int vz = (n.index / CHUNK_SIZE_X) % CHUNK_SIZE_Z;

 //       if (vx < 0 || vx >= CHUNK_SIZE_X ||
 //           vy < 0 || vy >= CHUNK_SIZE_Y ||
 //           vz < 0 || vz >= CHUNK_SIZE_Z)
	//		continue; // Continue next chunk

 //       Voxel& v = blocks[n.index];
 //       if (!g_BlockTypes[v.blockID].isTransparent)
 //           continue;

 //       if (v.lightLevel >= n.light)
 //           continue;

 //       v.lightLevel = n.light;

 //       if (n.light > 1) {
 //           for (int i = 0; i < 6; ++i) {
 //               int nx = vx + faceDirs[i][0];
 //               int ny = vy + faceDirs[i][1];
 //               int nz = vz + faceDirs[i][2];
	//			int neighborIndex = GetBlockIndex(nx, ny, nz);
	//			if (neighborIndex == -1) continue;
 //               q.push({ GetBlockIndex(nx, ny, nz), uint8_t(n.light - 1)});
 //           }
 //       }
    //}
}

uint8_t Chunk::GetLightLevel(int x, int y, int z)
{
    int blockIndex = GetBlockIndex(x, y, z);
    if (blockIndex == -1) return 15;
    return blocks[blockIndex].lightLevel;
}

inline uint8_t Chunk::GetLightLevel(unsigned int index)
{
    return blocks[index].lightLevel;
}

void Chunk::SetLightLevel(int x, int y, int z, uint8_t newLight)
{
    if (newLight > 15) return;
    blocks[GetBlockIndex(x, y, z)].lightLevel = newLight;
}

void Chunk::SetLightLevel(int index, uint8_t newLight)
{
    if (newLight > 15) return;
    blocks[index].lightLevel = newLight;
}
