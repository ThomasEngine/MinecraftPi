#pragma once
#include "Renderer.h"
#include "ICommand.h"
#include "Camera.h"

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
	Camera& GetCamera() { return m_Camera; }

private:
	Camera m_Camera;
	bool m_Flying{ true };
	float m_MovementSpeed;
};

