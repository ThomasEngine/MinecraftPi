
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

class ChunkManager;
class FastNoiseLite;
class Chunk {
public:
    glm::ivec3 chunkPos; // chunk grid position
    std::vector<uint8_t> blocks;
    std::unique_ptr<Mesh> mesh;
    bool isReady = false;

    Chunk(glm::ivec3 pos, FastNoiseLite& FNL);
    ~Chunk();

    void SetBlock(int x, int y, int z, uint8_t type);
    uint8_t GetBlock(int x, int y, int z) const;

    void createChunkMesh(Renderer& renderer, ChunkManager& owner);
    void Draw(Renderer& renderer, const glm::mat4& viewProj, const Shader& shader, const Texture& texture) const;


private:
    bool IsEmpty(int x, int y, int z) const;
};
