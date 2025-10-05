#include <iostream>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shaders/shaders.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "chunk.hpp"
#include "player.hpp"
#include "gamedata.hpp"
#include "worldGenerator.hpp"
#include "loader.hpp"


// Time global variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame = 0.0f;

// Player
Player player(glm::vec3(0,0,0), glm::vec3(0,0,-1), glm::vec3(0,1,0), 20.5f, 0.1f);

// world generator
WorldGenerator worldGen;

void loadTexture(const char *filename, unsigned int *texture);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void generateChunks(std::string seed, std::fstream &file, glm::ivec3 pos);
blockType getAir();

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


    // BUFFERS AND GEOMETRY -------------------------------------------------------------
    
    // Vertices and indices of active chunks
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    // Generates the buffers
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    // MOVEMENT ------------------------------------------------------------------

    // Sets mouse callback function for camera direction update
    glfwSetCursorPosCallback(window, mouse_callback);

    // SHADERS LOADING ------------------------------------------------------------------
    
    // Load shaders
    Shader baseShader("../shaders/base.vert", "../shaders/base.frag");

    // Finds locations of model, view and project matrix in base shader
    unsigned int baseModelLoc = glGetUniformLocation(baseShader.ID, "model");
	unsigned int baseViewLoc = glGetUniformLocation(baseShader.ID, "view");
	unsigned int baseProjectionLoc = glGetUniformLocation(baseShader.ID, "projection");


    // WORLD LOADING ------------------------------------------------------------------
   
    Chunk activeChunks[2*RENDER_DISTANCE + 1][2*RENDER_DISTANCE + 1][2*RENDER_DISTANCE + 1];

    // opens world file
    std::fstream worldFile("../world.dat", std::ios::in | std::ios::out | std::ios::app);

    // Cheks if file exists
    if (!worldFile) {
        std::cerr << "Error loading world file\n";
        return -1;
    }

    // Creates chunk index hash
    wl::buildChunkIndex(worldFile);

    // Loads first chunks
    wl::loadActiveChunks(player, worldGen, worldFile, activeChunks);
    // Loads vertices
    wl::loadActiveVertices(player, vertices, indices, activeChunks);

    // Binds buffers
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // Sends vertecies and indicies data to the buffers
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), indices.data(), GL_STATIC_DRAW);

    // Enables position attribute for shaders
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbinds
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    #ifdef DEBUG
    std::cout << "Loaded " << vertices.size() / 3 << " triangles\n";
    std::vector<float> times;
    std::vector<float> loadingChunksTimes;
    #endif


    // MAIN PROGRAM LOOP ----------------------------------------------------------------

    // Enables depth test for OpenGL to properly render
    glEnable(GL_DEPTH_TEST);

    // view and projection matrices
    glm::mat4 view = player.getView();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) WIDTH / HEIGHT, 0.1f, 100.0f);

    // Stores old plyaer chunk position
    glm::ivec3 oldChunkPos = player.getChunkPosition();

    while (!glfwWindowShouldClose(window)) {

        // Computing FPS
        currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
        
        // Input and movement
        player.processCameraMovement(window, deltaTime);
        
        // chunk loading: only loads if chunk position changed
        if (oldChunkPos != player.getChunkPosition())
        {
            #ifdef DEBUG
                std::cout << "Player chunk position: " << player.getChunkPosition().x << " " << player.getChunkPosition().y << " " << player.getChunkPosition().z << "\n";
                std::cout << "Loading chunks \n";
                float loadChunkTime = glfwGetTime();
            #endif

            // reloads chunks
            wl::loadActiveChunks(player, worldGen, worldFile, activeChunks);
            // reloads vertices
            wl::loadActiveVertices(player, vertices, indices, activeChunks);

            #ifdef DEBUG
                loadingChunksTimes.push_back(glfwGetTime() - loadChunkTime);
            #endif

            // Updates vertex data:
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

            // Sends vertecies and indicies data to the buffers
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), indices.data(), GL_STATIC_DRAW);

            // Enables position attribute for shaders
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // Unbinds
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        oldChunkPos = player.getChunkPosition();
        
        // color and buffer refresh
        glClearColor(0.1f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activates shader (for now all blocks have the same one)
        baseShader.Activate();

        // Sets view matrix
        view = player.getView();
        
        // Sets active chunks position
        glm::mat4 model(1.0f);
        model = glm::scale(model, glm::vec3(SCALE_FACTOR));
        model = glm::translate(model, glm::vec3((CHUNCK_SIZE)*activeChunks[RENDER_DISTANCE][RENDER_DISTANCE][RENDER_DISTANCE].getChunkPos()));

        // Assigns matrices values to shaders
        glUniformMatrix4fv(baseModelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(baseViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(baseProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Draws
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // Buffers swap and events -------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        #ifdef DEBUG
        times.push_back(deltaTime);
        #endif
    }

    #ifdef DEBUG
    float sum = 0;
    for (int i = 0; i < times.size(); i++)
    {
        sum += times[i];
    }
    std::cout << "DEBUG: Average frame FPS: " << times.size()/sum << std::endl;

    float sum2 = 0;
    for (int i = 0; i < loadingChunksTimes.size(); i++)
    {
        sum2 += loadingChunksTimes[i];
    }
    std::cout << "DEBUG: Average chunk loading time: " << sum2/loadingChunksTimes.size() << std::endl;
    #endif

    // Terminates the program
    worldFile.close();

    glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	baseShader.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
}

// Loads an OpenGL texture from a file
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

// mouse callback for camera movement
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    player.cameraMouseCallback(window, xpos, ypos);
}

blockType getAir() {
    int n = sizeof(b_blocks)/sizeof(blockType);
    return b_blocks[n - 1];
}