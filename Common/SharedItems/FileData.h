#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

struct BlockData {
	uint8_t id;
	unsigned int index;
};

struct WorldData {
	int seed; // World seed
	float timeOfDay; // Time of day in hours
	glm::ivec3 spawnPoint; // Current player spawn point
	glm::vec3 playerPosition; // Last known player position
};

class FileData
{
public:
	FileData(const char* path); // path to location to store chunks in different files (if path = 0, create new world)
	~FileData();

	void SaveChunkData(int cx, int cy, int cz, const std::vector<BlockData>& data);
	std::vector<BlockData> LoadChunkData(int cx, int cy, int cz);

	WorldData LoadWorldData();
	void SaveWorldData(const WorldData& data);

private:
	std::string basePath;
};

