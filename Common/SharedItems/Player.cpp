#include "Player.h"
#include "Camera.h"
#include "CollisionSystem.h"
#include "World.h"

Player::Player(int w, int h, Camera* m_Camera)
	: m_MovementSpeed(1.0f), m_CS(nullptr), m_Camera(m_Camera)
{
	SetPosition(glm::vec3(0.0f, 120.0f, 0.0f));
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
	UpdateMoveY(deltaTime);
}

void Player::MoveForward(float deltaTime)
{
	glm::vec3 dir = m_Camera->GetDirection();
	dir.y = 0;
	glm::vec3 move = glm::normalize(dir) * m_MovementSpeed * deltaTime;
	glm::vec3 pos = m_Camera->GetPosition();

	// X axis
	glm::vec3 tryX = pos + glm::vec3(move.x, 0, 0);
	if (!m_CS->CheckGridCollision(tryX))
		pos.x += move.x;

	// Z axis
	glm::vec3 tryZ = pos + glm::vec3(0, 0, move.z);
	if (!m_CS->CheckGridCollision(tryZ))
		pos.z += move.z;

	SetPosition(pos);
}

void Player::MoveBackward(float deltaTime)
{
	glm::vec3 dir = m_Camera->GetDirection();
	dir.y = 0;
	glm::vec3 move = glm::normalize(-dir) * m_MovementSpeed * deltaTime;
	glm::vec3 pos = m_Camera->GetPosition();

	// X axis
	glm::vec3 tryX = pos + glm::vec3(move.x, 0, 0);
	if (!m_CS->CheckGridCollision(tryX))
		pos.x += move.x;

	// Z axis
	glm::vec3 tryZ = pos + glm::vec3(0, 0, move.z);
	if (!m_CS->CheckGridCollision(tryZ))
		pos.z += move.z;

	SetPosition(pos);
}

void Player::MoveLeft(float deltatime)
{
	glm::vec3 right = m_Camera->GetRight(); 
	glm::vec3 move = right * m_MovementSpeed * deltatime;
	glm::vec3 pos = m_Camera->GetPosition();

	// X axis
	glm::vec3 tryX = pos + glm::vec3(move.x, 0, 0);
	if (!m_CS->CheckGridCollision(tryX))
		pos.x += move.x;

	// Z axis
	glm::vec3 tryZ = pos + glm::vec3(0, 0, move.z);
	if (!m_CS->CheckGridCollision(tryZ))
		pos.z += move.z;

	SetPosition(pos);
}

void Player::MoveRight(float deltaTime)
{
	glm::vec3 right = m_Camera->GetRight(); 
	glm::vec3 move = -right * m_MovementSpeed * deltaTime;
	glm::vec3 pos = m_Camera->GetPosition();

	// X axis
	glm::vec3 tryX = pos + glm::vec3(move.x, 0, 0);
	if (!m_CS->CheckGridCollision(tryX))
		pos.x += move.x;

	// Z axis
	glm::vec3 tryZ = pos + glm::vec3(0, 0, move.z);
	if (!m_CS->CheckGridCollision(tryZ))
		pos.z += move.z;

	SetPosition(pos);
}

void Player::Jump(float deltaTime)
{
	if (m_Flying)
	{
		glm::vec3 nextPos = m_Camera->GetPosition() + glm::vec3(0.f, 1.f, 0.f) * m_MovementSpeed * deltaTime;
		if (!m_CS->CheckGridCollision(nextPos))
			SetPosition(nextPos);
	}
	else
	{
		if (m_OnGround)
		{
			m_Vel.y = JUMP_SPEED;
			m_OnGround = false;
		}
	}
}

void Player::UpdateMoveY(float deltaTime)
{
	if (m_Flying) return;
	// Apply gravity
	m_Vel.y -= GRAVITY * deltaTime;
	if (m_Vel.y > MAX_FALL_SPEED) m_Vel.y =MAX_FALL_SPEED;

	// Create a new position to check for collisions
	glm::vec3 newPos = m_Camera->GetPosition();
	float changeY = m_Vel.y * deltaTime;
	newPos.y += changeY;

	// Check for collisions and update position if no collision
	if (!m_CS->CheckGridCollision(newPos))
	{
		SetPosition(newPos);
		m_OnGround = false; // in the air
	}
	else
	{
		m_Vel.y = 0;
		m_OnGround = true; // on the ground
	}
}

void Player::Crouch(float deltaTime)
{
	glm::vec3 nextPos = m_Camera->GetPosition() + glm::vec3(0.f, 1.f, 0.f) * -m_MovementSpeed * deltaTime;
	if (!m_CS->CheckGridCollision(nextPos))
		if (m_Flying)
			SetPosition(nextPos);
}

void Player::SetFlying(bool b)
{
	m_Flying = b;
}

void Player::SetPosition(const glm::vec3& pos)
{
	m_Pos = pos;
	m_Camera->SetPosition(pos);
}
