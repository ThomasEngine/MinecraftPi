#include "FileData.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>

FileData::FileData(const char* path)
{
    // Initialize basePath and ensure directory exists
    basePath = std::string("./saves/") + path;
    std::filesystem::create_directories(basePath);
}

FileData::~FileData()
{
    // Destructor
}

void FileData::SaveChunkData(int cx, int cy, int cz, const std::vector<BlockData>& data)
{
    std::string chunkPath = basePath + "/chunk_" + std::to_string(cx) + "_" + std::to_string(cy) + "_" + std::to_string(cz) + ".dat";
    std::ofstream ofs(chunkPath, std::ios::binary);
    if (!ofs) {
        std::cerr << "Failed to open file for writing: " << chunkPath << std::endl;
        return;
    }
    ofs.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(BlockData));
}

std::vector<BlockData> FileData::LoadChunkData(int cx, int cy, int cz)
{
    std::vector<BlockData> returningData;
    std::string chunkPath = basePath + "/chunk_" + std::to_string(cx) + "_" + std::to_string(cy) + "_" + std::to_string(cz) + ".dat";
    std::ifstream ifs(chunkPath, std::ios::binary | std::ios::ate);
    if (!ifs) {
        // File does not exist, return empty data
        return returningData;
    }
    std::streamsize fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    size_t numEntries = fileSize / sizeof(BlockData);
    returningData.resize(numEntries);
    if (!ifs.read(reinterpret_cast<char*>(returningData.data()), fileSize)) {
        std::cerr << "Failed to read file: " << chunkPath << std::endl;
        returningData.clear();
    }
    return returningData;
}

WorldData FileData::LoadWorldData()
{
    WorldData data{};
    std::string worldPath = basePath + "/world.dat";
    std::ifstream ifs(worldPath, std::ios::binary);
    if (!ifs) {
        // File does not exist, return default data
        return data;
    }
    ifs.read(reinterpret_cast<char*>(&data), sizeof(WorldData));
    return data;
}

void FileData::SaveWorldData(const WorldData& data)
{
    std::string worldPath = basePath + "/world.dat";
    std::ofstream ofs(worldPath, std::ios::binary);
    if (!ofs) {
        std::cerr << "Failed to open file for writing: " << worldPath << std::endl;
        return;
    }
    ofs.write(reinterpret_cast<const char*>(&data), sizeof(WorldData));
}
