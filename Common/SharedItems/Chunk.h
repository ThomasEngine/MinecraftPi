#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include <memory>
#include <queue>


extern const FaceVertex faceVertices[6][4];
extern const int faceDirs[6][3];

// Chunk dimensions
constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 128 * 2;
constexpr int CHUNK_SIZE_Z = 16;

struct AONeighbors {
    bool side1;
    bool side2;
    bool corner;
};

constexpr int CHUNKSIZE = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;

struct Voxel;
class ChunkLoader;
class FastNoiseLite;
class FileData;
struct BlockData;
class Chunk {
public:
    glm::ivec3 chunkPos; // chunk grid position
    std::vector<Voxel> blocks;
    std::queue<unsigned int> sunlightBfsQueue;
    std::unique_ptr<Mesh> mesh;
	std::unique_ptr<Mesh> transparentMesh;
    bool hasTransparentBlocks = false;
    Chunk(glm::ivec3 pos, ChunkLoader& owner, const std::vector<BlockData>& importData); 
    ~Chunk();

    void SetBlock(int x, int y, int z, uint8_t type);
    uint8_t GetBlock(int x, int y, int z) const;
	Voxel GetVoxel(int x, int y, int z) const;

	void NeigbourVoxelQueue(int x, int y, int z, ChunkLoader& owner);

    void destroyMesh(Renderer& ren);
    void createChunkMesh(Renderer& renderer, ChunkLoader& owner);
    void uploadChunkMesh(Renderer& renderer, ChunkLoader& owner);
	void createTransparentMesh(Renderer& renderer, ChunkLoader& owner);
	void createSolidMesh(Renderer& renderer, ChunkLoader& owner);
    void DrawSolid(Renderer& renderer, const glm::mat4& viewProj, const Shader& shader, const Texture& texture) const;
    void DrawTransparent(Renderer& renderer, const glm::mat4& viewProj, const Shader& shader, const Texture& texture) const;


    uint8_t GetLightLevel(int x, int y, int z);
    uint8_t GetLightLevel(unsigned int index);


    void SetLightLevel(int x, int y, int z, uint8_t lightLevel);
    void SetLightLevel(int index, uint8_t lightLevel);

	void ApplySunlight(ChunkLoader& owner);
	void PropagateLight(ChunkLoader& owner);
	void ReapplyBorderLight(ChunkLoader& owner);
	void SampleBorderLightFromNeighbors(ChunkLoader& owner);

	void PlaceTrees(Renderer& ren, ChunkLoader& owner);

    void ExportChangedBlocks(FileData& fileHelper);
private:
	std::vector<glm::ivec3> m_TreePositions;

	void GenerateTerrain(ChunkLoader& owner, const std::vector<BlockData>& loadedData);
	inline int GetBlockIndex(int x, int y, int z) const;

	std::vector<BlockData> changedBlocks;
	bool m_FinishedGeneration = false;


    bool IsEmpty(int x, int y, int z) const;
	bool NeighborIsEmpty(int nx, int ny, int nz, ChunkLoader& owner, int y) const;
	float FaceBrightness(FaceDirection face) const;
	AONeighbors GetAONeighbors(int face, int vertex, int x, int y, int z, ChunkLoader& owner, int localY) const;
    bool CanPlaceTreeAt(const glm::ivec3& loc) const;

};
