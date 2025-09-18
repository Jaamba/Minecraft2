#ifndef WORLD_GENERATOR
#define WORLD_GENERATOR

#include <string>
#include <random>
#include "gamedata.hpp"
#include <iostream>

class WorldGenerator
{
private:
    int m_seed;

    // Returns a random number between 0 and 1 given a position
    float randomNumber(int x, int y);
    // Returns a perlin noise value in a position (x, y) (relative to grid's closest down left corner,
    // which is in position (cx, cy)
    float perlin(int x, int y, int cx, int cy, int gridSize);
    float lerp(float x, float y, float t);

public:
    WorldGenerator();
    WorldGenerator(int seed);
    virtual ~WorldGenerator() = default;

    // Generates string for chunk at pos (x, y, z)
    BlockGrid genChunk(int x, int y, int z);
};

WorldGenerator::WorldGenerator() {
    m_seed = 0;
}
WorldGenerator::WorldGenerator(int seed) {
    m_seed = seed;
}

BlockGrid WorldGenerator::genChunk(int x, int y, int z) {
    // Idea: for each (x, y) in chunk's area, we compute a perlin value t(x, y)
    // for each block, if z > t(x, y) the block will be air, other wise it will
    // be grass or rock. For now, we assume gridSize = chunk_size. 

    // Generates perlin values
    float perlinValues[CHUNCK_SIZE][CHUNCK_SIZE];
    for (int i = 0; i < CHUNCK_SIZE; i++)
    {
        for (int j = 0; j < CHUNCK_SIZE; j++)
        {
            perlinValues[i][j] = 2*perlin(i, j, x, z, CHUNCK_SIZE);
        }
    }

    // Writes final chunk
    BlockGrid chunk;
    for (int i = 0; i < CHUNCK_SIZE; i++)
    {
        for (int j = 0; j < CHUNCK_SIZE; j++)
        {
            for (int k = 0; k < CHUNCK_SIZE; k++)
            {
                // Cheks if block is above or below ground
                if (y*CHUNCK_SIZE + j < ceil(perlinValues[i][k]) - 2)
                {
                    chunk.blocks[i][j][k] = b_blocks[1];
                }
                else if (y*CHUNCK_SIZE + j < ceil(perlinValues[i][k]))
                {
                    chunk.blocks[i][j][k] = b_blocks[0];
                }
                else {
                    chunk.blocks[i][j][k] = b_blocks[2];
                }
            }
        }
    }
    
    return chunk;
}

float WorldGenerator::randomNumber(int x, int y) {
    std::mt19937 gen1(x);
    std::mt19937 gen2(y); 
    std::uniform_real_distribution<float> dist(0, 1); 
    return (dist(gen1) + dist(gen2))/2;    
}

float WorldGenerator::perlin(int x, int y, int cx, int cy, int gridSize) {
    if (gridSize <= 1)
    {
        std::cerr << "Error: gridsize has to be at least 1\n";
        return -1;
    }
    
    // Angles of orientation for corner vectors
    float upRight, downRight, upLeft, downLeft;
    
    // Calculates random vectors
    downLeft = 2*PI*randomNumber(cx, cy);
    downRight = 2*PI*randomNumber(cx + gridSize, cy);
    upLeft = 2*PI*randomNumber(cx, cy + gridSize);
    upRight = 2*PI*randomNumber(cx + gridSize, cy + gridSize);

    // Offset vectors have coordinates:
    // upRight: (x - gridSize, y - gridSize)
    // downRight: (x - gridSize, y)
    // upLeft: (x, y - gridSize)
    // downleft: (x, y)

    // calculates dot products
    float downLeftd = x*cos(downLeft) + y*sin(downLeft);    
    float downRightd = (x - gridSize)*cos(downRight) + y*sin(downRight);
    float upLeftd = x*cos(upLeft) + (y - gridSize)*sin(upLeft);
    float upRightd = (x - gridSize)*cos(upRight) + (y - gridSize)*sin(upRight);

    // normalized position in grid
    float u = (float)(x)/gridSize;
    float v = (float)(y)/gridSize;

    float x1 = lerp(downLeftd, downRightd, u);
    float x2 = lerp(upLeftd, upRightd, u);

    return lerp(x1, x2, v);
}

float WorldGenerator::lerp(float x, float y, float t) {
    return y*t + (1-t)*x;
}

#endif