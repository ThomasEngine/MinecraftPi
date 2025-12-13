#pragma once
#include <glm/glm.hpp>

class CollisionSystem;
class Camera;
class Player
{
public:
	Player(int w, int h, Camera* cam);
	~Player();

	void AsignCamera(Camera* cam) { m_Camera = cam; }

	void Update(float deltaTime);
	void MoveForward(float deltaTime);
	void MoveBackward(float deltaTime);
	void MoveLeft(float deltatime);
	void MoveRight(float deltaTime);
	void Jump(float deltaTime);
	void UpdateMoveY(float deltaTime);
	void Crouch(float deltaTime);
	void SetFlying(bool b);
	void SetMoveSpeed(float s) { m_MovementSpeed = s; }
	void SetCollisionSystem(CollisionSystem* CS) { m_CS = CS; }
	Camera* GetCamera() { return m_Camera; }
	glm::vec3 getRect() const { return m_PlayerRect; }

private:
	CollisionSystem* m_CS;
	Camera* m_Camera;
	bool m_Flying{ false };
	bool m_OnGround{ false };
	float m_MovementSpeed;
	glm::vec3 m_Vel{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Pos{ 0.0f, 0.0f, 0.0f };

	glm::vec3 m_PlayerRect = { 0.8f, 1.9f, 0.6 };

	void SetPosition(const glm::vec3& pos);
};

