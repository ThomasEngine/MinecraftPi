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


Chunk::Chunk(glm::ivec3 pos, FastNoiseLite& FNL)
    : chunkPos(pos)
{
    blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z, 0);

	for (int x = 0; x < CHUNK_SIZE_X; ++x) {
		for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
			float noiseValue = FNL.GetNoise(float((chunkPos.x * CHUNK_SIZE_X) + x), float((chunkPos.z * CHUNK_SIZE_Z) + z));
			int height = static_cast<int>((noiseValue + 1.0f) / 2.0f * (CHUNK_SIZE_Y - 1)); // Normalize to [0, CHUNK_SIZE_Y-1]
			for (int y = 0; y <= height; ++y) {
				if (y == height) {
					SetBlock(x, y, z, B_GRASS);
				}
				else if (y > height - 5) {
					SetBlock(x, y, z, B_DIRT);
				}
				else {
					SetBlock(x, y, z, B_STONE);
				}
			}
		}
	}
}

Chunk::~Chunk()
{
}

void Chunk::SetBlock(int x, int y, int z, uint8_t type) {
    if (x < 0 || x >= CHUNK_SIZE_X ||
        y < -128 || y >= CHUNK_SIZE_Y ||
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

// Pseudocode plan:
// - The neighbor check block is missing a statement after the if (owner.GetBlockAtPosition(...) == B_AIR)
// - The if block is empty, so the compiler expects a statement, but finds only a closing brace
// - The logic should be: if the neighbor block is air, add the face; otherwise, skip to next face
// - For the else branch (when neighbor is inside the chunk), the logic is inverted: if not air, skip; else, add face
// - Fix: Add a continue statement after the neighbor air check, and invert the logic in the else branch

void Chunk::createChunkMesh(Renderer& renderer, ChunkManager& owner)
{
    if (isReady) {
        renderer.destroyMesh(*mesh);
    }

    std::vector<FaceVertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int indexOffset = 0;

    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                uint8_t blockId = GetBlock(x, y, z);
                if (blockId == B_AIR) continue;
                for (int face = 0; face < 6; ++face) {
                    int nx = x + faceDirs[face][0];
                    int ny = y + faceDirs[face][1];
                    int nz = z + faceDirs[face][2];
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

                        if (owner.GetBlockAtPosition(neighborBlockPos, neighborChunkPos) != B_AIR)
                            continue; 
                    }
                    else {
                        if (GetBlock(nx, ny, nz) != B_AIR)
                            continue;
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
                        float cellY = float(atlasIndex / cols);

                        cellY += 15;

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

void Chunk::Draw(Renderer& renderer, const glm::mat4& viewProj, const Shader& shader, const Texture& texture) const 
{
    if (isReady)
    {
        renderer.drawMesh(*mesh, shader, viewProj, texture);
    }
}



