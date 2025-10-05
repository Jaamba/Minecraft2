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

enum FaceDir {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

class Chunk
{
private:
    // Position of the chunk 
    int m_x, m_y, m_z;
    // Array of block types in the chunk
    BlockGrid* m_blockGrid;
    // Vector containing all the vertices and indices of the blocks
    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;

    // function to add a block to m_vertices
    // Position is relative to chunk position
    void addBlockVertices(glm::ivec3 pos);

    // Utility function for adding a face to
    // m_vertices
    void addFace(glm::ivec3 pos, FaceDir direction);

public:
    Chunk();
    Chunk(glm::ivec3 pos, BlockGrid blocks);
    Chunk(const Chunk& other);
    Chunk& operator=(const Chunk& other);
    virtual ~Chunk();

    // block get/set utilites
    blockType getBlock(int x, int y, int z) const;
    void setBlock(blockType type, int x, int y, int z);
    glm::ivec3 getChunkPos() const;

    // gets blocks verices and texture coordinates
    std::vector<float> getChunkVertices() const;
    std::vector<unsigned int> getChunkIndices() const;

    // Returns a translated version of vertices
    std::vector<float> translateVertices(glm::ivec3 offset) const;
    std::vector<unsigned int> translateIndices(unsigned int offset) const;

    // chunk generation
    void fill(blockType type);
};

Chunk::Chunk() {
    // Creates the chunk's memory on the heap
    m_blockGrid = new BlockGrid;
}

Chunk::Chunk(glm::ivec3 pos, BlockGrid blocks) {
    // assings positions
    m_x = pos.x; m_y = pos.y; m_z = pos.z;

    // Creates the chunk's memory on the heap
    m_blockGrid = new BlockGrid;
    *m_blockGrid = blocks;

    // Adds blocks' vertices
    for (int i = 0; i < CHUNCK_SIZE; i++) {
        for (int j = 0; j < CHUNCK_SIZE; j++) {
            for (int k = 0; k < CHUNCK_SIZE; k++) {
                if (!m_blockGrid->blocks[i][j][k].isAir)
                {
                    addBlockVertices(glm::ivec3(i, j, k));                
                }
            }
        }
    }
    
}

Chunk::Chunk(const Chunk& other) {
    m_x = other.m_x;
    m_y = other.m_y;
    m_z = other.m_z;

    m_blockGrid = new BlockGrid;
    *m_blockGrid = *other.m_blockGrid;

    m_vertices = other.getChunkVertices();
    m_indices = other.getChunkIndices();
}

Chunk& Chunk::operator=(const Chunk& other) {
    if (this != &other) { 
        m_x = other.m_x;
        m_y = other.m_y;
        m_z = other.m_z;

        // Frees old memory
        delete m_blockGrid;

        // reallocates and copies new memory
        m_blockGrid = new BlockGrid;
        *m_blockGrid = *other.m_blockGrid;

        m_vertices = other.getChunkVertices();
        m_indices = other.getChunkIndices();
    }
    return *this;
}

Chunk::~Chunk() {
    delete m_blockGrid;
}

// Returns a block at a given position
blockType Chunk::getBlock(int x, int y, int z) const {
    if(x >= CHUNCK_SIZE || y >= CHUNCK_SIZE || z >= CHUNCK_SIZE) {
        std::cerr << "Error: getBlock index cannot be larger than chunk size\n";
    }

    return m_blockGrid->blocks[x][y][z];
}

// Sets a block at a given position
void Chunk::setBlock(blockType type, int x, int y, int z) {
    if(x >= CHUNCK_SIZE || y >= CHUNCK_SIZE || z >= CHUNCK_SIZE) {
        std::cerr << "Error: setBlock index cannot be larger than chunk size\n";
    }

    m_blockGrid->blocks[x][y][z] = type;
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

void Chunk::addBlockVertices(glm::ivec3 pos) {
    // Adds all faces
    for (int i = 0; i < 6; i++)
    {
        // Adds the face
        addFace(pos, static_cast<FaceDir>(i));
    }
}

void Chunk::addFace(glm::ivec3 pos, FaceDir direction) {
    // How many vertices have already been made
    int startIndex = m_vertices.size() / 3;
    int x, y, z;
    x = pos.x, y = pos.y, z = pos.z;

    // Vertices of the face
    float v[4][3];

    switch (direction) {
        case FRONT:
            v[0][0] = x;   v[0][1] = y;   v[0][2] = z;
            v[1][0] = x+1; v[1][1] = y;   v[1][2] = z;
            v[2][0] = x+1; v[2][1] = y+1; v[2][2] = z;
            v[3][0] = x;   v[3][1] = y+1; v[3][2] = z;
            break;

        case BACK:
            v[0][0] = x+1; v[0][1] = y;   v[0][2] = z+1;
            v[1][0] = x;   v[1][1] = y;   v[1][2] = z+1;
            v[2][0] = x;   v[2][1] = y+1; v[2][2] = z+1;
            v[3][0] = x+1; v[3][1] = y+1; v[3][2] = z+1;
            break;

        case LEFT:
            v[0][0] = x; v[0][1] = y;   v[0][2] = z+1;
            v[1][0] = x; v[1][1] = y;   v[1][2] = z;
            v[2][0] = x; v[2][1] = y+1; v[2][2] = z;
            v[3][0] = x; v[3][1] = y+1; v[3][2] = z+1;
            break;

        case RIGHT:
            v[0][0] = x+1; v[0][1] = y;   v[0][2] = z;
            v[1][0] = x+1; v[1][1] = y;   v[1][2] = z+1;
            v[2][0] = x+1; v[2][1] = y+1; v[2][2] = z+1;
            v[3][0] = x+1; v[3][1] = y+1; v[3][2] = z;
            break;

        case BOTTOM: 
            v[0][0] = x;   v[0][1] = y; v[0][2] = z+1;
            v[1][0] = x+1; v[1][1] = y; v[1][2] = z+1;
            v[2][0] = x+1; v[2][1] = y; v[2][2] = z;
            v[3][0] = x;   v[3][1] = y; v[3][2] = z;
            break;

        case TOP: 
            v[0][0] = x;   v[0][1] = y+1; v[0][2] = z;
            v[1][0] = x+1; v[1][1] = y+1; v[1][2] = z;
            v[2][0] = x+1; v[2][1] = y+1; v[2][2] = z+1;
            v[3][0] = x;   v[3][1] = y+1; v[3][2] = z+1;
            break;
    }

    // Adds vertices to m_vertices
    for (int i = 0; i < 4; i++) {
        m_vertices.push_back(v[i][0]);
        m_vertices.push_back(v[i][1]);
        m_vertices.push_back(v[i][2]);
    }

    // Adds indices
    m_indices.push_back(startIndex + 0);
    m_indices.push_back(startIndex + 1);
    m_indices.push_back(startIndex + 2);

    m_indices.push_back(startIndex + 0);
    m_indices.push_back(startIndex + 2);
    m_indices.push_back(startIndex + 3);
}

std::vector<float> Chunk::getChunkVertices() const {
    return m_vertices;
}

std::vector<unsigned int> Chunk::getChunkIndices() const {
    return m_indices;
}

std::vector<float> Chunk::translateVertices(glm::ivec3 offset) const {
    std::vector<float> translatedVertices;

    for (int i = 0; i < m_vertices.size(); i += 3)
    {
        translatedVertices.push_back(m_vertices[i] + offset.x);
        translatedVertices.push_back(m_vertices[i + 1] + offset.y);
        translatedVertices.push_back(m_vertices[i + 2] + offset.z);
    }

    return translatedVertices;
}

std::vector<unsigned int> Chunk::translateIndices(unsigned int offset) const {
    std::vector<unsigned int> indices;
    for (int i = 0; i < m_indices.size(); i++)
    {
        indices.push_back(m_indices[i] + offset);
    }
    return indices;
}
#endif

