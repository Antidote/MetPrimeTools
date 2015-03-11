#include "core/CCamera.hpp"

#include <float.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GL/gl.h>

CCamera::CCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
      m_movementSpeed(5.0f),
      m_mouseSensitivity(0.25f),
      m_zoom(55.0f)
{
    m_position = position;
    m_worldUp = up;
    m_yaw = yaw;
    m_pitch = pitch;
    updateCameraVectors();
}

CCamera::CCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
      m_movementSpeed(1.5f),
      m_mouseSensitivity(0.25f),
      m_zoom(55.0f)
{
    m_position = glm::vec3(posX, posY, posZ);
    m_worldUp = glm::vec3(upX, upY, upZ);
    m_yaw = yaw;
    m_pitch = pitch;
    updateCameraVectors();
}

glm::mat4 CCamera::viewMatrix()
{
    return glm::lookAt(m_position, m_position + m_front, m_worldUp);
}

glm::mat4 CCamera::rotationMatrix()
{
    return glm::mat4(glm::quat(viewMatrix()));
}

glm::mat4 CCamera::projectionMatrix()
{
    return glm::perspective(glm::radians(m_zoom), (float)m_viewportWidth/(float)m_viewportHeight, 0.1f, 4096.0f);
}

void CCamera::processKeyboard(EMovement direction, float deltaTime)
{
    float velocity = m_movementSpeed * deltaTime;
    if (velocity > 20.f)
        velocity = 20.f;

    if(direction == FORWARD)
        m_position += m_front * velocity;
    if(direction == BACKWARD)
        m_position -= m_front * velocity;
    if(direction == LEFT)
        m_position -= m_right * velocity;
    if(direction == RIGHT)
        m_position += m_right * velocity;
}

void CCamera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if(constrainPitch)
    {
        if(m_pitch > 89.0f)
            m_pitch = 89.0f;
        if(m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Eular angles
    updateCameraVectors();
}

void CCamera::processMouseScroll(float yoffset)
{
    if(m_zoom >= 1.0f && m_zoom <= 45.0f)
        m_zoom -= yoffset;
    if(m_zoom <= 1.0f)
        m_zoom = 1.0f;
    if(m_zoom >= 45.0f)
        m_zoom = 45.0f;
}

void CCamera::setYaw(float yaw)
{
    m_yaw = yaw;
    updateCameraVectors();
}

void CCamera::setPitch(float pitch)
{
    m_pitch = pitch;
    updateCameraVectors();
}

void CCamera::setPosition(const glm::vec3& pos)
{
    m_position = pos;
    updateCameraVectors();
}

glm::vec3 CCamera::position() const
{
    return m_position;
}

void CCamera::setViewport(int width, int height)
{
    m_viewportWidth = width;
    m_viewportHeight = height;
}

glm::vec3 CCamera::front() const
{
    return m_front;
}

void CCamera::increaseSpeed()
{
    m_movementSpeed += 0.25f;
    if (m_movementSpeed > 5.0f)
        m_movementSpeed = 5.0f;
}

void CCamera::decreaseSpeed()
{
    m_movementSpeed -= 0.25f;
    if (m_movementSpeed < 0.05f)
        m_movementSpeed = 0.05f;
}

void CCamera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = -cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
    front.z = sin(glm::radians(m_pitch));
    front.y = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
    m_front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
