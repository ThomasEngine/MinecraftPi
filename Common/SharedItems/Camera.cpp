#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

Camera::Camera(int w, int h)
    : m_position(0.0f, 0.0f, 3.0f),
    m_rotation(0.0f, 0.0f, 0.0f),
    m_view(1.0f),
    m_proj(1.0f),
    m_isPerspective(true)
{
    float fov = 80.f;
    float aspect = w > 0 ? (float)w / (float)h : 1.0f;
    float nearPlane = 0.1f;
    float farPlane = 4000.f;

    SetPerspective(glm::radians(fov), aspect, nearPlane, farPlane);
    m_view = glm::lookAt(glm::vec3(2.5f, 1.5f, 7.5f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

void Camera::SetPosition(const glm::vec3& pos) 
{
    m_position = pos;
}

void Camera::SetRotation(const glm::vec3& eulerAngles) 
{
    m_rotation = eulerAngles;
}

void Camera::SetPerspective(float fov, float aspect, float near, float far) 
{
    m_proj = glm::perspective(fov, aspect, near, far);
    m_isPerspective = true;
}

void Camera::SetOrthographic(float left, float right, float bottom, float top, float near, float far) 
{
    m_proj = glm::ortho(left, right, bottom, top, near, far);
    m_isPerspective = false;
}

glm::vec3 Camera::GetPosition() const 
{
    return m_position;
}

glm::vec3 Camera::GetDirection() const
{
    glm::vec3 direction;
    direction.x = cos(m_pitch) * sin(m_yaw);
    direction.y = sin(m_pitch);
    direction.z = cos(m_pitch) * cos(m_yaw);
    return glm::normalize(direction);
}


glm::mat4 Camera::GetViewMatrix() const
{
    glm::vec3 direction;
    direction.x = cos(m_pitch) * sin(m_yaw);
    direction.y = sin(m_pitch);
    direction.z = cos(m_pitch) * cos(m_yaw);
	glm::vec3 position = m_position;
	position.y += .9f; 
    return glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));
}

glm::mat4 Camera::GetProjectionMatrix() const 
{
    return m_proj;
}

glm::mat4 Camera::GetViewProjectionMatrix() const 
{
    return m_proj * GetViewMatrix();
}

glm::vec3 Camera::GetRight() const
{
	return glm::normalize(glm::vec3(cos(m_yaw), 0, -sin(m_yaw)));
}

void Camera::MoveForward(float amount) 
{
    glm::vec3 forward = glm::normalize(glm::vec3(sin(m_yaw), 0, cos(m_yaw)));
    m_position += forward * amount;
}
void Camera::MoveRight(float amount) 
{
    glm::vec3 right = glm::normalize(glm::vec3(cos(m_yaw), 0, -sin(m_yaw)));
    m_position += right * amount;
}
void Camera::Move(glm::vec3 delta)
{
    m_position += delta;
}
void Camera::AddYaw(float delta) 
{ 
    m_yaw += delta; 
}

void Camera::AddPitch(float delta) 
{
    m_pitch += delta;
    m_pitch = glm::clamp(m_pitch, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f);
}

