#ifndef WORLD_LOADER
#define WORLD_LOADER

#include <iostream>
#include <fstream>
#include "chunk.hpp"
#include "gamedata.hpp"
#include <unordered_map>
#include <tuple>

// world loader namespace
namespace wl {

using ChunkKey = std::tuple<int, int, int>;

// Structs needed for position hashing functions
struct ChunkHeader {
    int x, y, z;
    uint32_t size; 
};

// Chunk hashing function
struct KeyHash {
    std::size_t operator()(const ChunkKey& k) const {
        auto [x,y,z] = k;
        return std::hash<long long>()(((long long)x<<40) ^ ((long long)y<<20) ^ (long long)z);
    }
};

// Hash equals function
struct KeyEq {
    bool operator()(const ChunkKey& a, const ChunkKey& b) const {
        return a == b;
    }
};

// Stores information about position of chunks in the file. Is loaded only on launch
inline std::unordered_map<std::tuple<int, int, int>, std::streampos, KeyHash, KeyEq> chunkIndex;

// Reads chunks from a file and loads them to activechunks
inline void loadActiveChunks(Player &player, WorldGenerator &generator, std::fstream &file, 
    Chunk (&activeChunks)[2*RENDER_DISTANCE+1][2*RENDER_DISTANCE+1][2*RENDER_DISTANCE+1]) {
    
    // Determines the position of the lower left corner chunk to be loaded
    int tx = floor(player.getChunkPosition().x - RENDER_DISTANCE);
    int ty = floor(player.getChunkPosition().y - RENDER_DISTANCE);
    int tz = floor(player.getChunkPosition().z - RENDER_DISTANCE);

    // Cicles in the active chunks
    int limit = 2*RENDER_DISTANCE + 1;
    for (int i = 0; i < limit; i++) {
        for (int j = 0; j < limit; j++) {
            for (int k = 0; k < limit; k++) {
                int x = tx + i;
                int y = ty + j;
                int z = tz + k;

                ChunkKey key = {x,y,z};

                if (chunkIndex.find(key) != chunkIndex.end()) {
                    // If the key is in the file, loads the chunk
                    // Moves to the key's position in the file
                    file.clear();
                    file.seekg(chunkIndex[key]);

                    // Reads the chunk's header
                    ChunkHeader header;
                    file.read(reinterpret_cast<char*>(&header), sizeof(header));

                    // Reads the chunk's data
                    BlockGrid data;
                    file.read(reinterpret_cast<char*>(&data), header.size);

                    // Adds the loaded chunk to activeChunks
                    activeChunks[i][j][k] = Chunk(glm::ivec3(x, y, z), data);
                } else {
                    // If the key is not in the file, creates the chunk
                    BlockGrid data = generator.genChunk(x,y,z);
                    activeChunks[i][j][k] = Chunk(glm::ivec3(x, y, z), data);

                    // Adds the chunk to the end of the file
                    file.clear();
                    file.seekp(0, std::ios::end);

                    uint32_t size = sizeof(data);
                    ChunkHeader header {x,y,z,size};
                    std::streampos pos = file.tellp();

                    // Writes the chunk header
                    file.write(reinterpret_cast<char*>(&header), sizeof(header));
                    
                    // Writes the chunk data
                    file.write(reinterpret_cast<char*>(&data), size);

                    // Updates the index
                    chunkIndex[key] = pos;
                }
            }
        }
    }
}

// Called on launch: Loads the chunk index
inline void buildChunkIndex(std::fstream &file) {
    #ifdef DEBUG
    std::cout << "Loading world index...\n";
    int n = 0;
    #endif
    
    // Resets file stream
    file.clear();
    file.seekg(0, std::ios::beg);

    while (true) {
        // Stores current position in file
        std::streampos pos = file.tellg();

        // Reads the chunk header
        ChunkHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(header));

        // stops if the end of the file has been reached
        if (!file) {
            #ifdef DEBUG
            std::cout << "Loaded world index: " << n << " chunks loaded successfully\n";
            #endif

            break;
        }

        // Skips to next chunk
        file.seekg(header.size, std::ios::cur);

        // Saves the current chunk in the index
        chunkIndex[{header.x, header.y, header.z}] = pos;
        n++;
    }
}

// Loads data from active chunks into vertices data
inline void loadActiveVertices(Player& player, std::vector<float>& vertices, std::vector<unsigned int>& indices, 
    Chunk (&activeChunks)[2*RENDER_DISTANCE+1][2*RENDER_DISTANCE+1][2*RENDER_DISTANCE+1]) {
    
    // Makes sure vectors are empty
    vertices.clear();
    indices.clear();

    // Cicles into active chunks
    for (int i = -RENDER_DISTANCE; i <= RENDER_DISTANCE; i++) {
    for (int j = -RENDER_DISTANCE; j <= RENDER_DISTANCE; j++) {
    for (int k = -RENDER_DISTANCE; k <= RENDER_DISTANCE; k++) {
        // Current active chunk
        Chunk* activeChunk = &(activeChunks[i + RENDER_DISTANCE][j + RENDER_DISTANCE][k + RENDER_DISTANCE]);

        // Position of active chunk relative to player
        glm::ivec3 relChunkPos = glm::ivec3(i, j, k)*(CHUNCK_SIZE);

        // translate chunks vertices
        std::vector<float> translatedVertices = activeChunk->translateVertices(relChunkPos);

        // Calculates how much to translate indices
        int toTranslate = vertices.size() / 3;

        // Loads vertices
        vertices.insert(vertices.end(), translatedVertices.begin(), translatedVertices.end());

        // translates indices
        std::vector<unsigned int> translatedIndices = activeChunk->translateIndices(toTranslate);
        
        // Loads indices
        indices.insert(indices.end(), translatedIndices.begin(), translatedIndices.end());
    }}}
}

}
#endif