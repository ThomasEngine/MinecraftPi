#include "Chunk.h"
#include <BlockRegistery.h>
#include <noise/FastNoiseLite.h>
#include "ChunkManager.h"

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
Chunk::Chunk(glm::ivec3 pos, FastNoiseLite& Continental, FastNoiseLite& Erosion, FastNoiseLite& PeaksValleys)
    : chunkPos(pos)
{
    blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z, 0);

    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
            // World coordinates
            float wx = float(chunkPos.x * CHUNK_SIZE_X + x);
            float wz = float(chunkPos.z * CHUNK_SIZE_Z + z);

            // Creating height
            float continental = Continental.GetNoise(wx, wz); 
            float erosion = Erosion.GetNoise(wx, wz);         
            float peaks = PeaksValleys.GetNoise(wx, wz);      

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
            int intHeight = static_cast<int>(height);

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
        }
    }
}

Chunk::~Chunk()
{
}

void Chunk::SetBlock(int x, int y, int z, uint8_t type) {
    if (x < 0 || x >= CHUNK_SIZE_X ||
        y < 0 || y >= CHUNK_SIZE_Y ||
        z < 0 || z >= CHUNK_SIZE_Z)
        return;
    blocks[x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z)] = type;
}

uint8_t Chunk::GetBlock(int x, int y, int z) const
{
    if (x < 0 || x >= CHUNK_SIZE_X ||
        y < 0 || y >= CHUNK_SIZE_Y ||
        z < 0 || z >= CHUNK_SIZE_Z)
        return 0;
    return blocks[x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z)];
}

bool Chunk::IsEmpty(int x, int y, int z) const
{
    return GetBlock(x, y, z) != 0 ? true : false;
}

void Chunk::destroyMesh(Renderer& ren)
{
	if (mesh) {
		ren.destroyMesh(*mesh);
	}
}

void Chunk::createChunkMesh(Renderer& renderer, ChunkManager& owner)
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
                if (g_BlockTypes[blockId].isTransparent && blockId != B_WATER) continue;

                // Only draw the top layer of water
                if (blockId == B_WATER) {
                    uint8_t aboveBlock = (y + 1 < CHUNK_SIZE_Y) ? GetBlock(x, y + 1, z) : B_AIR;
                    if (aboveBlock == B_WATER) continue;
                }

                for (int face = 0; face < 6; ++face) {
                    int nx = x + faceDirs[face][0];
                    int ny = y + faceDirs[face][1];
                    int nz = z + faceDirs[face][2];
                    if (ny < 0) continue;
                    if (nx < 0 || nx >= CHUNK_SIZE_X ||
                        nz < 0 || nz >= CHUNK_SIZE_Z)
                    {
                        glm::ivec3 neighborChunkPos = chunkPos;
                        if (nx < 0) neighborChunkPos.x -= 1;
                        else if (nx >= CHUNK_SIZE_X) neighborChunkPos.x += 1;
                        if (nz < 0) neighborChunkPos.z -= 1;
                        else if (nz >= CHUNK_SIZE_Z) neighborChunkPos.z += 1;
                        neighborChunkPos.y = 0;

                        glm::ivec3 neighborBlockPos = glm::ivec3(nx, y, nz);
                        neighborBlockPos.x = nx < 0 ? CHUNK_SIZE_X - 1 : (nx >= CHUNK_SIZE_X ? 0 : nx);
                        neighborBlockPos.z = nz < 0 ? CHUNK_SIZE_Z - 1 : (nz >= CHUNK_SIZE_Z ? 0 : nz);
                        uint8_t neighborChunkBlockId = owner.GetBlockAtPosition(neighborBlockPos, neighborChunkPos);
                        if (neighborChunkBlockId != B_AIR && neighborChunkBlockId != B_WATER)
                            continue; 
                    }
                    else {
                        uint8_t neighborBlockId = GetBlock(nx, ny, nz);
                        if (neighborBlockId != B_AIR && neighborBlockId != B_WATER)
                            continue;
                    }

                    if (blockId == B_WATER && face != 3) // Top face
                        continue;

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

                        vertices.push_back(FaceVertex{ pos, tex, cellX, cellY });
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
    isReady = true;
    vertices.clear();
    indices.clear();
}

void Chunk::createTransparentMesh(Renderer& renderer, ChunkManager& owner)
{

}

void Chunk::createSolidMesh(Renderer& renderer, ChunkManager& owner)
{

}

void Chunk::Draw(Renderer& renderer, const glm::mat4& viewProj, const Shader& shader, const Texture& texture) const 
{
    if (!mesh) return;
    renderer.drawMesh(*mesh, shader, viewProj, texture);
}
