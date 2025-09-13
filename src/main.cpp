#include <iostream>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shaders/shaders.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "chunk.h"
#include "camera.hpp"

#define PI 4*atan(1)

// window size (not resizable)
#define WIDTH 800
#define HEIGHT 800
#define SCALE_FACTOR 1

#define RENDER_DISTANCE 4

/* Struttura del file mondo
xyzid xyzid xyzid ...
000id 001id 002id ...

funzione loadActiveChunks(&activechunks, position, file)
*/

/*
    Idea: Un chunk è un array tridimensionale di block_type.
    ad ogni frame si itera tra gli elementi del chunk e si renderizza il 
    blocco con una funzione render_block(chunk c, int i, int j).
    In questo modo avviene la parte di rendering.
    Per la parte fisica si itera tra i blocchi aggiornando il blocco corrente
    e i blocchi circostanti in base all'id del block_type. Se per esempio
    blocco.id == "sabbia" si aggiorna la posizione del blocco in modo tale
    da farlo cadere. 
*/

/* BLOCK TYPE LIST ---------------------------------------------------------------------- */
//                       ID       isAir       hasGravity
blockType b_dirt      = {0,       false,      false};
blockType b_stone     = {1,       false,      false};
blockType b_air       = {2,       true,       false};

/* TEXTURE ID FILENAME LIST --------------------------------------------------------------*/
struct idTexture
{
    unsigned int ID;
    const char* filename;
};

idTexture t_texturesIDs[] =
{
    /* 0 - dirt */  {0, "../textures/0_dirt.jpg"},
    /* 1 - stone */ {1, "../textures/1_stone.jpg"}
};

// Block geometry info
float b_vertices[] = {
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
unsigned int b_indices[] = {
    0, 1, 2,  2, 3, 0,
    4, 5, 6,  6, 7, 4,
    8, 9,10, 10,11, 8,
    12,13,14, 14,15,12,
    16,17,18, 18,19,16,
    20,21,22, 22,23,20
};

// Time global variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame = 0.0f;

// Player
Camera player(glm::vec3(0,0,0), glm::vec3(0,0,-1), glm::vec3(0,1,0), 2.5f, 0.1f);

void loadTexture(const char *filename, unsigned int *texture);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void loadActiveChunks(const char *filename, Chunk (&activeChunks)[2*RENDER_DISTANCE+1][2*RENDER_DISTANCE+1][2*RENDER_DISTANCE+1]);

int main() {
    std::cout << "hello minecraft 2\n";

    // GLFW WINDOW CREATION -------------------------------------------------------------
    glfwInit();

    // Tells GLFW to use OpenGL version 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Makes window not resizable
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Minecraft 2", NULL, NULL);

    // Checks for window failures
    if (window == NULL) {
        std::cout << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // hides cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // loads GLAD for OpenGL
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // Speciefies OpenGL viewport
    glViewport(0, 0, WIDTH, HEIGHT);


    // BUFFERS --------------------------------------------------------------------------
    
    // defines the buffers. Index 0 for static blocks, 1 for dynamics
    GLuint VAO[2], VBO[2], EBO[2];

    // Generates the buffers
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(2, EBO);

    // Binds the buffers for static blocks
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);

    // Sends vertecies and indicies data to the buffers
    glBufferData(GL_ARRAY_BUFFER, sizeof(b_vertices), b_vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(b_indices), b_indices, GL_STATIC_DRAW);

    // Enables position and uv attributes for shaders
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbinds
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Binds the buffers for dynamic blocks
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);

    // Sends vertecies and indicies data to the buffers
    glBufferData(GL_ARRAY_BUFFER, sizeof(b_vertices), b_vertices, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(b_indices), b_indices, GL_DYNAMIC_DRAW);

    // Enables position and uv attributes for shaders
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbinds
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    // TEXTURE LOADING ------------------------------------------------------------------
    
    // Counts textured blocks
    int texturedBlocksN = sizeof(t_texturesIDs)/sizeof(t_texturesIDs[0]);
    unsigned int textures[texturedBlocksN];

    // Pre-loads textures and stores them in textures[]
    for (int i = 0; i < texturedBlocksN; i++)
    {
        loadTexture(t_texturesIDs[i].filename, &textures[i]);
    }
    
   
    // MOVEMENT ------------------------------------------------------------------

    // Creates the camera
    glm::vec3 playerChunkPos = player.getPosition()/((float)CHUNCK_SIZE);

    // Sets mouse callback function for camera direction update
    glfwSetCursorPosCallback(window, mouse_callback);

    // SHADERS LOADING ------------------------------------------------------------------
    
    // Load shaders
    Shader baseShader("../shaders/base.vert", "../shaders/base.frag");

    // Finds locations of model, view and project matrix in base shader
    unsigned int baseModelLoc = glGetUniformLocation(baseShader.ID, "model");
	unsigned int baseViewLoc = glGetUniformLocation(baseShader.ID, "view");
	unsigned int baseProjectionLoc = glGetUniformLocation(baseShader.ID, "projection");


    // CHUNKS CREATION ------------------------------------------------------------------
    
    Chunk activeChunks[2*RENDER_DISTANCE + 1][2*RENDER_DISTANCE + 1][2*RENDER_DISTANCE + 1];


    // MAIN PROGRAM LOOP ----------------------------------------------------------------

    // Enables depth test for OpenGL to properly render
    glEnable(GL_DEPTH_TEST);

    // view and projection matrices
    glm::mat4 view = player.getView();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) WIDTH / HEIGHT, 0.1f, 100.0f);
    
    while (!glfwWindowShouldClose(window)) {

        // Computing FPS
        currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        
        // Input and movement
        player.processCameraMovement(window, deltaTime);
        playerChunkPos = player.getPosition()/((float)CHUNCK_SIZE);
        
        
        // chunk loading
        loadActiveChunks("../world.dat", activeChunks);


        // color and buffer refresh
        glClearColor(0.1f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activates shader (for now all blocks have the same one)
        baseShader.Activate();

        // Chunk rendering
        int limit = 2*RENDER_DISTANCE + 1;  // numero di valori per asse
        for (int n = 0; n < limit * limit * limit; n++) {
            int cx = (n / (limit * limit)) - RENDER_DISTANCE;
            int cy = ((n / limit) % limit )- RENDER_DISTANCE;
            int cz = (n % limit) - RENDER_DISTANCE;
            
            Chunk activeChunk = activeChunks[cx + RENDER_DISTANCE][cy + RENDER_DISTANCE][cz + RENDER_DISTANCE];
            
            for (int m = 0; m < CHUNCK_SIZE * CHUNCK_SIZE * CHUNCK_SIZE; m++) {
                int i = m / (CHUNCK_SIZE * CHUNCK_SIZE);
                int j = (m / CHUNCK_SIZE) % CHUNCK_SIZE;
                int k = m % CHUNCK_SIZE;

                blockType activeBlock = activeChunk.getBlock(i, j, k);

                // Ignores air rendering
                if(activeBlock.isAir) continue;

                // Translates model to bottom left corner of chunk
                glm::mat4 model(1.0f);
                model = glm::scale(model, glm::vec3(SCALE_FACTOR));
                model = glm::translate(model, (float)CHUNCK_SIZE*activeChunk.getChunkPos() + glm::vec3(i, j, k));

                // Sets view to look at active chunk
                view = player.getView();

                // Assigns matrices values to shaders
                glUniformMatrix4fv(baseModelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniformMatrix4fv(baseViewLoc, 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(baseProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
                
                // Binds block texture
                glBindTexture(GL_TEXTURE_2D, textures[activeBlock.ID]);

                // Binds dynamic VAO if the block has gravity
                if (activeBlock.hasGravity) {
                    glBindVertexArray(VAO[1]);
                }
                else {
                    glBindVertexArray(VAO[0]);
                }

                // Renders the block
                //std::cout << "drawing block...\n";
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            }
        }


        // Buffers swap and events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminates the program
    glfwDestroyWindow(window);
    glfwTerminate();
}

void loadTexture(const char *filename, unsigned int *texture) {
    // Creates and binds the OpenGL object
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

	// sets filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Loads the image
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    
    // Generates the OpenGL texture
	if(data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load the texture" << std::endl;
	}

    // Removes the image texture from memory
	stbi_image_free(data);

    // Unbinds the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    player.cameraMouseCallback(window, xpos, ypos);
}

// Reads chunks from a file and loads them to activechunks
void loadActiveChunks(const char *filename, Chunk (&activeChunks)[2*RENDER_DISTANCE+1][2*RENDER_DISTANCE+1][2*RENDER_DISTANCE+1]) {
    std::ifstream file(filename);

    // Cheks if file exists
    if (!file) {
        std::cerr << "Error loading world file\n";
        return;
    }

    // x, y and z of the bottom left corner of the chunks to load
    int tx, ty, tz;
    glm::vec3 playerChunkPos = player.getPosition() / ((float)CHUNCK_SIZE);
    tx = round(playerChunkPos.x - RENDER_DISTANCE);
    ty = round(playerChunkPos.y - RENDER_DISTANCE);
    tz = round(playerChunkPos.z - RENDER_DISTANCE);

    // keeps track of loaded chunks
    bool loadedChunks[2*RENDER_DISTANCE+1][2*RENDER_DISTANCE+1][2*RENDER_DISTANCE+1] = {};

    // Current x, y and z
    int x, y, z;
    std::string chunk;
    while (file >> x >> y >> z >> chunk)
    {
        // Checks if chunk to be loaded has been found
        if ((x >= tx && x <= tx + 2*RENDER_DISTANCE) && 
            (y >= ty && y <= ty + 2*RENDER_DISTANCE) && 
            (z >= tz && z <= tz + 2*RENDER_DISTANCE))
        {
            Chunk tmp(x,y,z,chunk);
            activeChunks[x - tx]
                        [y - ty]
                        [z - tz] = tmp;

            loadedChunks[x - tx]
                        [y - ty]
                        [z - tz] = true;
        }
    }
    file.close();

    // Handles chunks not present in file
    int limit = 2*RENDER_DISTANCE + 1;
    for (int m = 0; m < limit * limit * limit; m++) {
        int i = m / (limit * limit);
        int j = (m / limit) % limit;
        int k = m % limit;

        if (!loadedChunks[i][j][k])
        {
            Chunk empty(glm::vec3(
                i - RENDER_DISTANCE + playerChunkPos.x,
                j - RENDER_DISTANCE + playerChunkPos.y,
                k - RENDER_DISTANCE + playerChunkPos.z
            ));
            empty.fill(b_air);

            activeChunks[i][j][k] = empty;
        }
    }
}