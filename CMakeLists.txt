cmake_minimum_required(VERSION 3.10.0)
project(minecraft2 VERSION 0.1.0 LANGUAGES C CXX)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

add_executable(minecraft2
    src/Shader.cpp
    src/main.cpp 
    src/glad.c
    src/chunk.cpp)
    

target_link_libraries(minecraft2 
    glfw
    OpenGL::GL)

target_include_directories(minecraft2 PRIVATE
    include)