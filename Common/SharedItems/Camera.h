#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    Camera(int w, int h);

    // Setters
    void SetPosition(const glm::vec3& pos);
    void SetRotation(const glm::vec3& eulerAngles);
    void SetLookAt(const glm::vec3& target);
    void SetPerspective(float fov, float aspect, float near, float far);
    void SetOrthographic(float left, float right, float bottom, float top, float near, float far);

    // Getters
    glm::vec3 GetPosition() const;
    glm::vec3 GetDirection() const;
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewProjectionMatrix() const;
	glm::vec3 GetRight() const;

    // Controls
    void MoveForward(float amount);
    void MoveRight(float amount);
    void Move(glm::vec3 delta);


    void AddYaw(float delta);
    void AddPitch(float delta);

private:
    glm::vec3 m_position;
    glm::vec3 m_rotation; 
    glm::mat4 m_view;
    glm::mat4 m_proj; 
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    bool m_isPerspective;
};
