
#include <glm/glm.hpp>
#include <vector>
#include "Rendering/include/Renderer.h"
#include "Rendering/include/Shader.h"
#include "Rendering/include/Texture.h"
#include <memory>
#include <queue>



// Chunk dimensions
constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 128 * 2;
constexpr int CHUNK_SIZE_Z = 16;

constexpr int CHUNKSIZE = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;

struct Voxel;
class ChunkManager;
class FastNoiseLite;
class Chunk {
public:
    glm::ivec3 chunkPos; // chunk grid position
    std::vector<Voxel> blocks;
    //Voxel blocks[CHUNKSIZE];
    std::queue<unsigned int> sunlightBfsQueue;
    std::unique_ptr<Mesh> mesh;
	std::unique_ptr<Mesh> transparentMesh;
    bool hasTransparentBlocks = false;
    Chunk(glm::ivec3 pos, ChunkManager& owner);
    ~Chunk();

    void SetBlock(int x, int y, int z, uint8_t type);
    uint8_t GetBlock(int x, int y, int z) const;
	Voxel GetVoxel(int x, int y, int z) const;

    void destroyMesh(Renderer& ren);
    void createChunkMesh(Renderer& renderer, ChunkManager& owner);
	void createTransparentMesh(Renderer& renderer, ChunkManager& owner);
	void createSolidMesh(Renderer& renderer, ChunkManager& owner);
    void DrawSolid(Renderer& renderer, const glm::mat4& viewProj, const Shader& shader, const Texture& texture) const;
    void DrawTransparent(Renderer& renderer, const glm::mat4& viewProj, const Shader& shader, const Texture& texture) const;


    uint8_t GetLightLevel(int x, int y, int z);
    uint8_t GetLightLevel(unsigned int index);


    void SetLightLevel(int x, int y, int z, uint8_t lightLevel);
    void SetLightLevel(int index, uint8_t lightLevel);

	void ApplySunlight(ChunkManager& owner);
	void PropagateLight(ChunkManager& owner);
	void ReapplyBorderLight(ChunkManager& owner);

private:
	void GenerateTerrain(ChunkManager& owner);
	inline int GetBlockIndex(int x, int y, int z) const;


    bool IsEmpty(int x, int y, int z) const;
	bool NeighborIsEmpty(int nx, int ny, int nz, ChunkManager& owner, int y) const;
	float FaceBrightness(FaceDirection face) const;
};
