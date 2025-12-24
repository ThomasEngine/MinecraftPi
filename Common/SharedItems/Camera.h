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
	void SetSprintFov(bool isSprinting);
	void SetAspectRatio(float aspect);

    // Getters
    glm::vec3 GetPosition() const;
    glm::vec3 GetDirection() const;
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewProjectionMatrix() const;
	glm::vec3 GetRight() const;

	// Rotation adjustments
    void AddYaw(float delta);
    void AddPitch(float delta);

    // Update
	void Update(float deltaTime);

private:
    glm::vec3 m_Position;
    glm::vec3 m_Rotation; 
    glm::mat4 m_View;
    glm::mat4 m_Proj; 
	float m_Fov = 70;
	float m_TargetFov = 70;
    float m_Yaw = 0.0f;
    float m_Pitch = 0.0f;
    bool m_IsPerspective;
};
