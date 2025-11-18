#include "Player.h"
#include "Camera.h"

Player::Player(int w, int h)
	: m_MovementSpeed(5.0f),
	m_Camera(w, h)
{
	m_Camera.SetPosition(glm::vec3(0.0f, 70.0f, 0.0f));
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{

}

void Player::MoveForward(float deltaTime)
{
	m_Camera.MoveForward(m_MovementSpeed);
}

void Player::MoveBackward(float deltaTime)
{
	m_Camera.MoveForward(-m_MovementSpeed);
}

void Player::MoveLeft(float deltatime)
{
	m_Camera.MoveRight(m_MovementSpeed);
}

void Player::MoveRight(float deltaTime)
{
	m_Camera.MoveRight(-m_MovementSpeed);
}

void Player::Jump(float deltaTime)
{
	if (m_Flying)
		m_Camera.Move(glm::vec3(0, m_MovementSpeed, 0));
}

void Player::Crouch()
{
	if (m_Flying)
		m_Camera.Move(glm::vec3(0, -m_MovementSpeed, 0));
}

void Player::SetFlying(bool b)
{
}
