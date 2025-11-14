#include "Chunk.h"
#include <array>

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
    { { {0,0,0}, {0,0} }, { {0,0,1}, {0,1} }, { {0,1,1}, {1,1} }, { {0,1,0}, {1,0} } },
    // +X (Right)
    { { {1,0,0}, {0,0} }, { {1,1,0}, {1,0} }, { {1,1,1}, {1,1} }, { {1,0,1}, {0,1} } }
};

Chunk::Chunk(glm::ivec3 pos)
    : chunkPos(pos)
{
    mesh = nullptr;

    blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z, 0);
    for (int x = 0; x < CHUNK_SIZE_X; ++x)
        for (int y = 0; y < CHUNK_SIZE_Y / 2; ++y)
            for (int z = 0; z < CHUNK_SIZE_Z; ++z)
                SetBlock(x, y, z, rand() % 2 + 1);
}

void Chunk::SetBlock(int x, int y, int z, unsigned int type) {
    if (x < 0 || x >= CHUNK_SIZE_X ||
        y < -128 || y >= CHUNK_SIZE_Y ||
        z < 0 || z >= CHUNK_SIZE_Z)
        return;
    blocks[x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z)] = type;
}

unsigned int Chunk::GetBlock(int x, int y, int z) const
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

void Chunk::createChunkMesh(Renderer& renderer)
{
    if (mesh) {
        renderer.destroyMesh(*mesh);
    }

    std::vector<FaceVertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int indexOffset = 0;

    const int faceDirs[6][3] = {
        {  0,  0, -1 }, // -Z (Back)
        {  0,  0,  1 }, // +Z (Front)
        {  0, -1,  0 }, // -Y (Bottom)
        {  0,  1,  0 }, // +Y (Top)
        { -1,  0,  0 }, // -X (Left)
        {  1,  0,  0 }  // +X (Right)
    };

    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                if (GetBlock(x, y, z) == 0) continue;
                for (int face = 0; face < 6; ++face) {
                    int nx = x + faceDirs[face][0];
                    int ny = y + faceDirs[face][1];
                    int nz = z + faceDirs[face][2];
                    if (IsEmpty(nx, ny, nz)) continue;

                    // Add 4 vertices for each face
                    for (int v = 0; v < 4; ++v) {
                        const FaceVertex& fv = faceVertices[face][v];
                        glm::vec3 pos = fv.pos + glm::vec3(x, y, z) + glm::vec3(chunkPos.x * CHUNK_SIZE_X, chunkPos.y * CHUNK_SIZE_Y, chunkPos.z * CHUNK_SIZE_Z);
                        glm::vec2 tex = fv.tex;
                        vertices.push_back(FaceVertex{ pos, tex, GetBlock(x, y, z) + 16 * 15, static_cast<uint8_t>(face), 1});
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

void Chunk::Draw(Renderer& renderer, const glm::mat4& viewProj, const Shader& shader, const Texture& texture) const 
{
    renderer.drawMesh(*mesh, shader, viewProj, texture);
}


