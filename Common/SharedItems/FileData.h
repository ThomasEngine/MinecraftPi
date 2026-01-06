#pragma once
#include <vector>
#include <string>		

struct BlockData {
	uint8_t id;
	unsigned int index;
};

struct WorldData {
	int seed;
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

