#include "chunk.h"

blockType Chunk::getBlock(int x, int y, int z) {
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