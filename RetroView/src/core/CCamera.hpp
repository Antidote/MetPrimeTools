#ifndef CCAMERA_HPP
#define CCAMERA_HPP

#include <vector>
#include <MathLib.hpp>
#include "CFrustum.hpp"

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class CCamera
{
public:
    enum EMovement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    // Constructor with vectors
    explicit CCamera(CVector3f position = CVector3f(0.0f, 0.0f, 0.0f), CVector3f up = CVector3f(0.0f, 0.0f, 1.0f), float yaw = -90.0f, float pitch = 0.0f);
    // Constructor with scalar values
    CCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    CTransform view() const;
    CTransform rotation() const;
    CQuaternion rotationAsQuaternion() const;
    const CProjection& projection() const;

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processKeyboard(EMovement direction, float deltaTime);

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseDolly(float offset);
    void processMouseStrafe(float offset);

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void processMouseScroll(float yoffset);

    void setYaw(float yaw);
    void setPitch(float pitch);
    void setPosition(const CVector3f& pos);
    const CVector3f& position() const;
    void setRotation(const CVector3f& pos);

    void setViewport(int width, int height);

    const CVector3f& front() const;

    void setMovementSpeed(float speed);
    float movementSpeed() const;

    void changeSpeed(float delta);


private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors();

    // Camera Attributes
    CVector3f m_position;
    CVector3f m_front;
    CVector3f m_up;
    CVector3f m_right;
    CVector3f m_worldUp;
    int m_viewportWidth;
    int m_viewportHeight;
    // Eular Angles
    float m_yaw;
    float m_pitch;
    // Camera options
    float m_movementSpeed;
    float m_mouseSensitivity;
    
    // Projection parameters (perspective)
    CProjection m_projection;
};

#endif // CAMERA_HPP
