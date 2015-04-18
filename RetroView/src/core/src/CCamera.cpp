#include <core/CCamera.hpp>

#include <float.h>

#include "core/GLInclude.hpp"

CCamera::CCamera(CVector3f position, CVector3f up, float yaw, float pitch)
    : m_front(CVector3f(0.0f, 0.0f, -1.0f)),
      m_movementSpeed(0.5f),
      m_mouseSensitivity(0.25f),
      m_projection(SProjPersp(Math::degToRad(55.0f)))
{
    m_position = position;
    m_worldUp = up;
    m_yaw = yaw;
    m_pitch = pitch;
    updateCameraVectors();
}

CCamera::CCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : m_front(CVector3f(0.0f, 0.0f, -1.0f)),
      m_movementSpeed(1.0f),
      m_mouseSensitivity(0.25f),
      m_projection(SProjPersp(Math::degToRad(55.0f)))
{
    m_position = CVector3f(posX, posY, posZ);
    m_worldUp = CVector3f(upX, upY, upZ);
    m_yaw = yaw;
    m_pitch = pitch;
    updateCameraVectors();
}

CTransform CCamera::view() const
{
    return Math::lookAt(m_position, m_position + m_front, m_worldUp);
}

CTransform CCamera::rotation() const
{
    CTransform ret = view();
    ret.m_origin.zeroOut();
    return ret;
}

CQuaternion CCamera::rotationAsQuaternion() const
{
    return CQuaternion(m_pitch, 0.0f, m_yaw);
}

const CProjection& CCamera::projection() const
{
    return m_projection;
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
    if (direction == UP)
        m_position += m_worldUp * velocity;
    if (direction == DOWN)
        m_position -= m_worldUp * velocity;
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

void CCamera::processMouseDolly(float offset)
{
    float velocity = offset * m_movementSpeed;
    m_position += m_front * velocity;
}

void CCamera::processMouseStrafe(float offset)
{
    float velocity = offset * m_movementSpeed;
    m_position += m_right * velocity;
}


void CCamera::processMouseScroll(float yoffset)
{
    SProjPersp persp = m_projection.getPersp();
    if(persp.m_fov >= 1.0f && persp.m_fov <= 45.0f)
        persp.m_fov -= yoffset;
    if(persp.m_fov <= 1.0f)
        persp.m_fov = 1.0f;
    if(persp.m_fov >= 45.0f)
        persp.m_fov = 45.0f;
    m_projection.setPersp(persp);
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

void CCamera::setPosition(const CVector3f& pos)
{
    m_position = pos;
    updateCameraVectors();
}

const CVector3f& CCamera::position() const
{
    return m_position;
}

void CCamera::setRotation(const CVector3f& rot)
{
    CQuaternion tmp(Math::degToRad(rot));
    m_yaw   = tmp.yaw();
    m_pitch = tmp.pitch();
    updateCameraVectors();
}

void CCamera::setViewport(int width, int height)
{
    SProjPersp persp = m_projection.getPersp();
    m_viewportWidth = width;
    m_viewportHeight = height;
    persp.m_aspect = (float)((float)width/(float)height);
    m_projection.setPersp(persp);
}

const CVector3f& CCamera::front() const
{
    return m_front;
}

void CCamera::setMovementSpeed(float speed)
{
    m_movementSpeed = speed;
    if (m_movementSpeed > 1.0f)
        m_movementSpeed = 1.0f;
    if (m_movementSpeed < 0.0075f)
        m_movementSpeed = 0.0075f;
}

float CCamera::movementSpeed() const
{
    return m_movementSpeed;
}

void CCamera::changeSpeed(float delta)
{
    m_movementSpeed += 0.045f * delta;

    if (m_movementSpeed > 1.0f)
        m_movementSpeed = 1.0f;
    if (m_movementSpeed < 0.0075f)
        m_movementSpeed = 0.0075f;
}

void CCamera::updateCameraVectors()
{
    // Calculate the new Front vector
    CVector3f front;
    front.x = -cos(Math::degToRad(m_pitch)) * cos(Math::degToRad(m_yaw));
    front.z = sin(Math::degToRad(m_pitch));
    front.y = cos(Math::degToRad(m_pitch)) * sin(Math::degToRad(m_yaw));
    m_front = front.normalized();
    // Also re-calculate the Right and Up vector
    m_right = m_front.cross(m_worldUp).normalized();  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_up = m_right.cross(m_front).normalized();
    
    // Update frustum planes
}
