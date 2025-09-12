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

#define PI 4*atan(1)

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
blockType b_air       = {0,       true,       false};
blockType b_dirt      = {1,       false,      false};

// window size (not resizable)
#define WIDTH 800
#define HEIGHT 800

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

    // loads GLAD for OpenGL
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // Speciefies OpenGL viewport
    glViewport(0, 0, WIDTH, HEIGHT);

                                                                                        
    // Main program loop ----------------------------------------------------------------

    // Enables depth test for OpenGL to properly render
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        // color and buffer refresh
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Buffers swap and events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminates the program
    glfwDestroyWindow(window);
    glfwTerminate();
}