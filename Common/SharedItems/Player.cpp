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
	m_CS->IsInWater(m_Camera->GetPosition(), m_PlayerRect) ? SetInWater(true) : SetInWater(false);
	Move(deltaTime);

}

void Player::Move(float deltaTime)
{
	UpdateMoveXZ(deltaTime);
	UpdateMoveY(deltaTime);
}

void Player::UpdateMoveXZ(float deltaTime)
{
	m_WishDir.y = 0;

	if (glm::length(m_WishDir) > 0.0f)
	{
		m_WishDir = glm::normalize(m_WishDir);
	}

	// Apply acceleration and friction
	float accel = m_OnGround ? GROUND_ACCEL : AIR_ACCEL;
	float friction = m_OnGround ? GROUND_FRICTION : AIR_FRICTION;
	float maxSpeed = m_OnGround ? MAX_GROUND_SPEED : MAX_AIR_SPEED;
	// if in water
	if (m_InWater)
	{
		accel = WATER_ACCEL;
		friction = WATER_FRICTION;
		maxSpeed = MAX_WATER_SPEED;
	}

	// if sprint
	if (m_Sprinting)
	{
		accel *= SPRINT_MULTIPLIER;
		maxSpeed *= SPRINT_MULTIPLIER;
	}

	// if crouch
	if (m_Crouching && m_OnGround)
	{
		accel *= CROUCH_MULTIPLIER;
		maxSpeed *= CROUCH_MULTIPLIER;
	}

	glm::vec3 velXZ = glm::vec3(m_Vel.x, 0.0f, m_Vel.z);
	glm::vec3 wishVel = m_WishDir * maxSpeed;
	glm::vec3 accelVec = wishVel - velXZ;
	float accelLen = glm::length(accelVec);
	if (accelLen > 0.0f) {
		accelVec = glm::normalize(accelVec) * accel * deltaTime;
		if (glm::length(accelVec) > accelLen)
			accelVec = glm::normalize(accelVec) * accelLen;
		m_Vel.x += accelVec.x;
		m_Vel.z += accelVec.z;
	}


	// Try to move x
	glm::vec3 newPos = m_Camera->GetPosition();
	newPos.x += m_Vel.x * deltaTime;
	if (!m_CS->CheckGridCollision(newPos, m_PlayerRect))
	{
		if (m_Crouching && m_OnGround)
		{
			newPos.y += m_Crouching ? -0.5f : 0.0f;
			if (m_CS->CheckGridCollision(newPos, m_PlayerRect))
			{
				newPos.y += m_Crouching ? 0.5f : 0.0f;
				SetPosition(newPos);
			}
			else
				m_Vel.x = 0;
		}
		else
		{
			SetPosition(newPos);
		}
	}
	else
		m_Vel.x = 0;

	// Try to move z
	newPos = m_Camera->GetPosition();
	newPos.z += m_Vel.z * deltaTime;
	if (!m_CS->CheckGridCollision(newPos, m_PlayerRect))
	{
		if (m_Crouching && m_OnGround)
		{
			newPos.y += m_Crouching ? -0.5f : 0.0f;
			if (m_CS->CheckGridCollision(newPos, m_PlayerRect))
			{
				newPos.y += m_Crouching ? 0.5f : 0.0f;
				SetPosition(newPos);
			}
			else
				m_Vel.z = 0;
		}
		else
		{
			SetPosition(newPos);
		}
	}
	else
		m_Vel.z = 0;

	// Apply friction
	m_Vel.x -= m_Vel.x * friction * deltaTime;
	m_Vel.z -= m_Vel.z * friction * deltaTime;

	m_WishDir = glm::vec3(0.0f);
}

void Player::MoveForward(float deltaTime)
{
	m_WishDir += m_Camera->GetDirection();
}

void Player::MoveBackward(float deltaTime)
{
	m_WishDir -= m_Camera->GetDirection();
}

void Player::MoveLeft(float deltatime)
{
	m_WishDir += m_Camera->GetRight();
}

void Player::MoveRight(float deltaTime)
{
	m_WishDir -= m_Camera->GetRight();
}

void Player::Jump(float deltaTime)
{
	if (m_Flying)
	{
		glm::vec3 nextPos = m_Camera->GetPosition() + glm::vec3(0.f, 1.f, 0.f) * m_MovementSpeed * deltaTime;
		if (!m_CS->CheckGridCollision(nextPos, m_PlayerRect))
			SetPosition(nextPos);
	}
	else
	{
		if (CanJump())
		{
			float jumpBoost = m_InWater ? JUMP_SPEED * 0.2f : JUMP_SPEED;
			m_Vel.y = jumpBoost;
			m_OnGround = false;
		}
	}
}

void Player::UpdateMoveY(float deltaTime)
{
	if (m_Flying) return;
	// Apply gravity

	float grav = m_InWater ? GRAVITY * 0.1f : GRAVITY;
	float maxFall = m_InWater ? MAX_FALL_SPEED * 0.1f : MAX_FALL_SPEED;

	m_Vel.y -= grav * deltaTime;
	if (m_Vel.y > maxFall) m_Vel.y = maxFall;

	// Create a new position to check for collisions
	glm::vec3 newPos = m_Camera->GetPosition();
	float changeY = m_Vel.y * deltaTime;
	newPos.y += changeY;

	// Check for collisions and update position if no collision
	if (!m_CS->CheckGridCollision(newPos, m_PlayerRect))
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
	if (!m_CS->CheckGridCollision(nextPos, m_PlayerRect))
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

bool Player::CanJump() const
{
	if (m_InWater) return true;
	if (m_Flying) return true;
	if (m_OnGround) return true;
	return false;
}
