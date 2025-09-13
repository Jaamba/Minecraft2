#ifndef CHUNCK
#define CHUNCK

#define CHUNCK_SIZE 3

#include <iostream>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gamedata.hpp"

class Chunk
{
private:
    // Array of block types in the chunk
    blockType m_blocks[CHUNCK_SIZE][CHUNCK_SIZE][CHUNCK_SIZE];
    int m_x, m_y, m_z;

public:
    bool isActive = false;
    
    // Creates empty chunk at given chunk coordinates
    Chunk();
    Chunk(int x, int y, int z, std::string s);
    Chunk(glm::vec3 pos);
    virtual ~Chunk() = default;

    // block get/set utilites
    blockType getBlock(int x, int y, int z) const;
    void setBlock(blockType type, int x, int y, int z);
    glm::vec3 getChunkPos() const;
    
    // chunk generation
    void fill(blockType type);
};

Chunk::Chunk() {
}

Chunk::Chunk(int x, int y, int z, std::string s) {
    m_x = x; m_y = y; m_z = z;

    std::stringstream ss(s);
    std::string token;
    
    int n = CHUNCK_SIZE*CHUNCK_SIZE*CHUNCK_SIZE;
    int i = 0;
    int bx, by, bz;
    unsigned int blockIds[CHUNCK_SIZE][CHUNCK_SIZE][CHUNCK_SIZE];

    while (std::getline(ss, token, '-') && i < n) {
        bx = i / (CHUNCK_SIZE * CHUNCK_SIZE);
        by = (i / CHUNCK_SIZE) % CHUNCK_SIZE;
        bz = i % CHUNCK_SIZE;

        m_blocks[bx][by][bz] = b_blocks[std::stoi(token)];
        i++;
    }
}

Chunk::Chunk(glm::vec3 pos) {
    m_x = (int)pos.x; m_y = (int)pos.y; m_z = (int)pos.z;
}

blockType Chunk::getBlock(int x, int y, int z) const {
    if(x >= CHUNCK_SIZE || y >= CHUNCK_SIZE || z >= CHUNCK_SIZE) {
        std::cerr << "Error: getBlock index cannot be larger than chunk size\n";
    }

    return m_blocks[x][y][z];
}

void Chunk::setBlock(blockType type, int x, int y, int z) {
    if(x >= CHUNCK_SIZE || y >= CHUNCK_SIZE || z >= CHUNCK_SIZE) {
        std::cerr << "Error: setBlock index cannot be larger than chunk size\n";
    }

    m_blocks[x][y][z] = type;
}

void Chunk::fill(blockType type) {
    for (int i = 0; i < CHUNCK_SIZE; i++)
    {
        for (int j = 0; j < CHUNCK_SIZE; j++)
        {
            for (int k = 0; k < CHUNCK_SIZE; k++)
            {
                setBlock(type, i, j, k);
            }
        }
    }
}

glm::vec3 Chunk::getChunkPos() const{
    return glm::vec3(m_x, m_y, m_z);
}

#endif

