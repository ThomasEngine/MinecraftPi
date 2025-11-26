#include "Player.h"
#include "Camera/include/Camera.h"
#include "CollisionSystem.h"

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
	glm::vec3 nextPos = m_Camera.GetPosition() + m_Camera.GetDirection() * m_MovementSpeed;
	if (!m_CS->CheckGridCollision(nextPos))
		m_Camera.MoveForward(m_MovementSpeed);
}

void Player::MoveBackward(float deltaTime)
{
	glm::vec3 nextPos = m_Camera.GetPosition() + m_Camera.GetDirection() * -m_MovementSpeed;
	if (!m_CS->CheckGridCollision(nextPos))
		m_Camera.MoveForward(-m_MovementSpeed);
}

void Player::MoveLeft(float deltatime)
{
	glm::vec3 nextPos = m_Camera.GetPosition() + m_Camera.GetDirection() * m_MovementSpeed;
	if (!m_CS->CheckGridCollision(nextPos))
		m_Camera.MoveRight(m_MovementSpeed);
}

void Player::MoveRight(float deltaTime)
{
	glm::vec3 nextPos = m_Camera.GetPosition() + m_Camera.GetDirection() * -m_MovementSpeed;
	if (!m_CS->CheckGridCollision(nextPos))
		m_Camera.MoveRight(-m_MovementSpeed);
}

void Player::Jump(float deltaTime)
{
	glm::vec3 nextPos = m_Camera.GetPosition() + glm::vec3(0.f, 1.f, 0.f) * -m_MovementSpeed;
	if (!m_CS->CheckGridCollision(nextPos))
	if (m_Flying)
		m_Camera.Move(glm::vec3(0, m_MovementSpeed, 0));
}

void Player::Crouch()
{
	glm::vec3 nextPos = m_Camera.GetPosition() + glm::vec3(0.f, 1.f, 0.f) * -m_MovementSpeed;
	if (!m_CS->CheckGridCollision(nextPos))
		if (m_Flying)
			m_Camera.Move(glm::vec3(0, -m_MovementSpeed, 0));
}

void Player::SetFlying(bool b)
{
}
