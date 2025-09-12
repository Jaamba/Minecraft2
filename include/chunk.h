#ifndef CHUNCK
#define CHUNCK

#define CHUNCK_SIZE 100

#include <iostream>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct blockType
{
    unsigned int ID;
    bool isAir;
    bool hasGravity;    
};

class Chunk
{
private:
    // Array of block types in the chunk
    blockType m_blocks[CHUNCK_SIZE][CHUNCK_SIZE][CHUNCK_SIZE];
    int m_x, m_y, m_z;

public:
    bool isActive = false;
    
    // Creates empty chunk at given chunk coordinates
    Chunk() = default;
    Chunk(int x, int y, int z);
    Chunk(glm::vec3 pos);
    virtual ~Chunk() = default;

    // block get/set utilites
    blockType getBlock(int x, int y, int z) const;
    void setBlock(blockType type, int x, int y, int z);
    glm::vec3 getChunkPos();
    
    // chunk generation
    void fill(blockType type);
};

#endif

