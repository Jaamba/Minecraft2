#ifndef GAME_DATA
#define GAME_DATA

// window size (not resizable)
#define WIDTH 800
#define HEIGHT 800
#define SCALE_FACTOR 1

#define RENDER_DISTANCE 2

struct blockType
{
    unsigned int ID;
    bool isAir;
    bool hasGravity;    
};

/* BLOCK TYPE LIST ---------------------------------------------------------------------- */
//                       ID       isAir       hasGravity
inline blockType b_dirt      = {0,       false,      false};
inline blockType b_stone     = {1,       false,      false};
inline blockType b_air       = {2,       true,       false};

/* TEXTURE ID FILENAME LIST --------------------------------------------------------------*/
struct idTexture
{
    unsigned int ID;
    const char* filename;
};

inline idTexture t_texturesIDs[] =
{
    /* 0 - dirt */  {0, "../textures/0_dirt.jpg"},
    /* 1 - stone */ {1, "../textures/1_stone.jpg"}
};

// BLOCK GEOMETRY INFO ----------------------------------------------------------------------
inline float b_vertices[] = {
    // position           // UV
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 0
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 1
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 2
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 3
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 4
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 5
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 6
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 7
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 8
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, // 9
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 10
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, // 11
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 12
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, // 13
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 14
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, // 15
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 16
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 17
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, // 18
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, // 19
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, // 20
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, // 21
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 22
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f  // 23
};
inline unsigned int b_indices[] = {
    0, 1, 2,  2, 3, 0,
    4, 5, 6,  6, 7, 4,
    8, 9,10, 10,11, 8,
    12,13,14, 14,15,12,
    16,17,18, 18,19,16,
    20,21,22, 22,23,20
};

#endif