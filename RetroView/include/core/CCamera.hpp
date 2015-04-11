#ifndef CCAMERA_HPP
#define CCAMERA_HPP

#include <vector>
#include <glm/glm.hpp>


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class CCamera
{
public:
    enum EMovement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    // Constructor with vectors
    explicit CCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f), float yaw = -90.0f, float pitch = 0.0f);
    // Constructor with scalar values
    CCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 viewMatrix();
    glm::mat4 rotationMatrix();
    glm::mat4 projectionMatrix();

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processKeyboard(EMovement direction, float deltaTime);

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void processMouseScroll(float yoffset);

    void setYaw(float yaw);
    void setPitch(float pitch);
    void setPosition(const glm::vec3& pos);
    glm::vec3 position() const;

    void setViewport(int width, int height);

    glm::vec3 front() const;

    void setMovementSpeed(float speed);
    float movementSpeed() const;

    void increaseSpeed();
    void decreaseSpeed();


private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors();

    // Camera Attributes
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;
    int m_viewportWidth;
    int m_viewportHeight;
    // Eular Angles
    float m_yaw;
    float m_pitch;
    // Camera options
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;
};

#endif // CAMERA_HPP
