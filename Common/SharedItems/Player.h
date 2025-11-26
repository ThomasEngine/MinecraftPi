#pragma once
#include "Rendering/include/Renderer.h"
#include "Commands/ICommand.h"
#include "Camera/include/Camera.h"

class CollisionSystem;
class Player
{
public:
	Player(int w, int h);
	~Player();

	void Update(float deltaTime);
	void MoveForward(float deltaTime);
	void MoveBackward(float deltaTime);
	void MoveLeft(float deltatime);
	void MoveRight(float deltaTime);
	void Jump(float deltaTime);
	void Crouch();
	void SetFlying(bool b);
	void SetMoveSpeed(float s) { m_MovementSpeed = s; }
	void SetCollisionSystem(CollisionSystem* CS) { m_CS = CS; }
	Camera& GetCamera() { return m_Camera; }

private:
	CollisionSystem* m_CS;
	Camera m_Camera;
	bool m_Flying{ true };
	float m_MovementSpeed;
};

