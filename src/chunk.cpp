#include "chunk.h"

Chunk::Chunk(int x, int y, int z) {
    m_x = x; m_y = y; m_z = z;
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

glm::vec3 Chunk::getChunkPos() {
    return glm::vec3(m_x, m_y, m_z);
}