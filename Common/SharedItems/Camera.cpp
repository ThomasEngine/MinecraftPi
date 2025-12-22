#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

Camera::Camera(int w, int h)
    : m_Position(0.0f, 0.0f, 3.0f),
    m_Rotation(0.0f, 0.0f, 0.0f),
    m_View(1.0f),
    m_Proj(1.0f),
    m_IsPerspective(true)
{
	// Default perspective projection will ad settings later
	float fov = 70.f; // initial FOV
	float aspect = w > 0 ? (float)w / (float)h : 1.0f; // aspect ratio w and h are from window size
	// Near and far planes
    float nearPlane = 0.2f;
    float farPlane = 256.f;


	// Initialize projection matrix
    SetPerspective(glm::radians(fov), aspect, nearPlane, farPlane);
	// Initialize view matrix
    m_View = glm::lookAt(glm::vec3(2.5f, 1.5f, 7.5f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

void Camera::SetPosition(const glm::vec3& pos) // Set camera position
{
    m_Position = pos;
}

void Camera::SetRotation(const glm::vec3& eulerAngles) // Set camera rotation in Euler angles (radians) 
{
    m_Rotation = eulerAngles;
}

void Camera::SetPerspective(float fov, float aspect, float near, float far) // Set perspective projection 
{
    m_Proj = glm::perspective(fov, aspect, near, far);
    m_IsPerspective = true;
}

void Camera::SetOrthographic(float left, float right, float bottom, float top, float near, float far) // Set orthographic projection 
{
    m_Proj = glm::ortho(left, right, bottom, top, near, far);
    m_IsPerspective = false;
}

void Camera::SetSprintFov(bool isSprinting) 
{
	// Target fov. Not directly set to allow smooth transition
	if (isSprinting && m_IsPerspective)
	{
		m_TargetFov = 85; 
	}
	else if (!isSprinting && m_IsPerspective)
	{
		m_TargetFov = 70;
	}
}

glm::vec3 Camera::GetPosition() const 
{
    return m_Position;
}

glm::vec3 Camera::GetDirection() const
{
	// Calculate forward direction vector from yaw and pitch
    glm::vec3 direction;
    direction.x = cos(m_Pitch) * sin(m_Yaw);
    direction.y = sin(m_Pitch);
    direction.z = cos(m_Pitch) * cos(m_Yaw);
    return glm::normalize(direction);
}


glm::mat4 Camera::GetViewMatrix() const
{
	// Calculate forward direction vector from yaw and pitch
    glm::vec3 direction;
    direction.x = cos(m_Pitch) * sin(m_Yaw);
    direction.y = sin(m_Pitch);
    direction.z = cos(m_Pitch) * cos(m_Yaw);
	glm::vec3 position = m_Position;
	position.y += .8f; // eye height offset 
    return glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));
}

glm::mat4 Camera::GetProjectionMatrix() const 
{
    return m_Proj;
}

glm::mat4 Camera::GetViewProjectionMatrix() const 
{
    return m_Proj * GetViewMatrix();
}

glm::vec3 Camera::GetRight() const
{
	return glm::normalize(glm::vec3(cos(m_Yaw), 0, -sin(m_Yaw)));
}

void Camera::AddYaw(float delta) 
{ 
    m_Yaw += delta; 
}

void Camera::AddPitch(float delta) 
{
    m_Pitch += delta;
    m_Pitch = glm::clamp(m_Pitch, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f);
}

void Camera::Update(float deltaTime)
{
	// Update target FOV smoothly
	if (m_IsPerspective 
        && (int)m_TargetFov != (int)m_Fov)
	{
       float fovChangeSpeed = 50.f; 
        if (m_Fov < m_TargetFov)
        {
            m_Fov += (fovChangeSpeed * deltaTime);
            if (m_Fov > m_TargetFov)
                m_Fov = m_TargetFov;
        }
        else
        {
            m_Fov -= (fovChangeSpeed * deltaTime);
            if (m_Fov < m_TargetFov)
                m_Fov = m_TargetFov;
        }
        m_Proj = glm::perspective(glm::radians(m_Fov), m_Proj[1][1] / m_Proj[0][0], 0.2f, 256.f);
	}
}

