#ifndef CHUNCK
#define CHUNCK


#include <iostream>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include "gamedata.hpp"

// Struct for storing the grid of blocks
struct BlockGrid {
    blockType blocks[CHUNCK_SIZE][CHUNCK_SIZE][CHUNCK_SIZE];
};

class Chunk
{
private:
    // Position of the chunk 
    int m_x, m_y, m_z;
    // Array of block types in the chunk
    BlockGrid m_blockGrid;

public:
    // Creates empty chunk at given chunk coordinates
    Chunk();
    Chunk(glm::ivec3 pos, BlockGrid blocks);
    virtual ~Chunk() = default;

    // block get/set utilites
    blockType getBlock(int x, int y, int z) const;
    void setBlock(blockType type, int x, int y, int z);
    glm::ivec3 getChunkPos() const;

    // chunk generation
    void fill(blockType type);
};

Chunk::Chunk() {
}

Chunk::Chunk(glm::ivec3 pos, BlockGrid blocks) {
    // assings positions
    m_x = pos.x; m_y = pos.y; m_z = pos.z;

    // Creates the chunk's memory on the heap
    m_blockGrid = blocks;
}

// Copy constructor
// Chunk::Chunk(const Chunk& other) {
//     m_x = other.m_x;
//     m_y = other.m_y;
//     m_z = other.m_z;

//     m_blockGrid = new BlockGrid;
//     *m_blockGrid = *other.m_blockGrid;
// }

// Copy assignment operator
// Chunk& Chunk::operator=(const Chunk& other) {
//     if (this != &other) { 
//         m_x = other.m_x;
//         m_y = other.m_y;
//         m_z = other.m_z;

//         // Frees old memory
//         delete m_blockGrid;

//         // reallocates and copies new memory
//         m_blockGrid = new BlockGrid;
//         *m_blockGrid = *other.m_blockGrid;
//     }
//     return *this;
// }

// Chunk::~Chunk() {
//     delete m_blockGrid;
// }

// Returns a block at a given position
blockType Chunk::getBlock(int x, int y, int z) const {
    if(x >= CHUNCK_SIZE || y >= CHUNCK_SIZE || z >= CHUNCK_SIZE) {
        std::cerr << "Error: getBlock index cannot be larger than chunk size\n";
    }

    return m_blockGrid.blocks[x][y][z];
}

// Sets a block at a given position
void Chunk::setBlock(blockType type, int x, int y, int z) {
    if(x >= CHUNCK_SIZE || y >= CHUNCK_SIZE || z >= CHUNCK_SIZE) {
        std::cerr << "Error: setBlock index cannot be larger than chunk size\n";
    }

    m_blockGrid.blocks[x][y][z] = type;
}

// Fills the chunk with one blocktype
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

// Returns chunk's position
glm::ivec3 Chunk::getChunkPos() const{
    return glm::ivec3(m_x, m_y, m_z);
}

#endif

