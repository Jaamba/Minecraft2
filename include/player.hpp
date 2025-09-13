#ifndef PLAYER
#define PLAYER

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Player
{
private:
    glm::vec3 m_position;
    glm::vec3 m_chunkPosition;
    glm::vec3 m_up;
    glm::vec3 m_front;
    float m_speed;
    float m_sensitivity;
    float m_lastX, m_lastY;
    float m_pitch, m_yaw, m_roll;
    bool m_firstMouse;
public:
    Player();
    Player(glm::vec3 position, glm::vec3 front, glm::vec3 up, float speed, float sensitivity);
    virtual ~Player() = default;

    // Utilites get / set functions
    glm::mat4 getView() const;
    glm::vec3 getPosition() const;
    glm::vec3 getChunkPosition() const;

    void cameraMouseCallback(GLFWwindow *window, float xpos, float ypos);
    void processCameraMovement(GLFWwindow *window, float deltaTime);
};

// Generates a camera at (0, 0, 0)
Player::Player()
{
    m_position = glm::vec3(0, 0, 0);
    m_up = glm::vec3(0, 1, 0);
    m_front = glm::vec3(0, 0, -1);

    // default values
    m_speed = 0.25; 
    m_sensitivity = 0.1;
    m_firstMouse = true;
    m_pitch = 0.0f, m_yaw = -90.0f;
}

// Generates a camera at a given position and orientation
Player::Player(glm::vec3 position, glm::vec3 front, glm::vec3 up, float speed, float sensitivity)
{
    m_position = position;
    m_front = front;
    m_up = up;
    m_speed = speed;
    m_sensitivity = sensitivity;

    // Calculates starting pitch, yaw and roll values
    m_roll = glm::degrees(acos(glm::dot(up, glm::vec3(0, 1, 0))));
    m_pitch = glm::degrees(asin(glm::dot(up, front)));
    m_yaw = glm::degrees(acos(glm::dot(front, glm::vec3(1, 0, 0))));
}

// Returns camera's view matrix
glm::mat4 Player::getView() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

// Returns player's position
glm::vec3 Player::getPosition() const {
    return m_position;
}

// Updates camera's orientation based on mouse position on window
void Player::cameraMouseCallback(GLFWwindow *window, float xpos, float ypos) {
    
    // Cheks if mouse entered the window for the first time
    if (m_firstMouse)
    {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
    }
  
    // Calculates mouse offset
    float xoffset = xpos - m_lastX;
    float yoffset = m_lastY - ypos; 
    m_lastX = xpos;
    m_lastY = ypos;

    // Updates pitch and yaw value
    xoffset *= m_sensitivity;
    yoffset *= m_sensitivity;
    m_yaw   += xoffset;
    m_pitch += yoffset;

    // Checks pitch limits
    if(m_pitch > 89.0f)
        m_pitch = 89.0f;
    if(m_pitch < -89.0f)
        m_pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    direction.y = sin(glm::radians(m_pitch));
    direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    
    // Updates camera's front
    m_front = glm::normalize(direction);
}

// Updates camera's position based on WASD keys and speed
void Player::processCameraMovement(GLFWwindow *window, float deltaTime) {
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		m_position += m_speed*deltaTime*m_front;
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		m_position -= m_speed*deltaTime*glm::normalize(glm::cross(m_front, m_up));
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		m_position -= m_speed*deltaTime*m_front;
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		m_position += m_speed*deltaTime*glm::normalize(glm::cross(m_front, m_up));
	}
}

glm::vec3 Player::getChunkPosition() const {
    return m_position/((float)CHUNCK_SIZE) + glm::vec3(1/((float)2*CHUNCK_SIZE));
}

#endif