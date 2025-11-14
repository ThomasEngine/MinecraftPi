
#include <glm/glm.hpp>
#include <vector>
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include <memory>

// Chunk dimensions
constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 128;
constexpr int CHUNK_SIZE_Z = 16;

// BlockTypeEnmus

enum Block
{

};

class Chunk {
public:
    glm::ivec3 chunkPos; // chunk grid position
    std::vector<unsigned int> blocks; // 0 = air, >0 = solid (block type)
    std::unique_ptr<Mesh> mesh;

    Chunk(glm::ivec3 pos);

    void SetBlock(int x, int y, int z, unsigned int type);
    unsigned int GetBlock(int x, int y, int z) const;

    void createChunkMesh(Renderer& renderer);
    void Draw(Renderer& renderer, const glm::mat4& viewProj, const Shader& shader, const Texture& texture) const;

private:
    bool IsEmpty(int x, int y, int z) const;
};
