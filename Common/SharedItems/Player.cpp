#include "Player.h"
#include "Camera.h"
#include "CollisionSystem.h"
#include "World.h"
#include "UITypes.h"





Player::Player(Camera* m_Camera)
	: m_MovementSpeed(1.0f), m_CS(nullptr), m_Camera(m_Camera)
{
	SetPosition(glm::vec3(0.0f, 120.0f, 0.0f));
	m_Inventory = new Container();
	m_Inventory->AddItem(I_GRASS_BLOCK, 1, 0);
	m_Inventory->AddItem(I_DIRT_BLOCK, 1, 1);
	m_Inventory->AddItem(I_STICK, 1, 2);
	m_Inventory->AddItem(I_WOODEN_PICKAXE, 1, 3);
	m_Inventory->AddItem(I_OAK_LOG_BLOCK, 1, 4);
	m_Inventory->AddItem(I_CRAFTING_TABLE, 1, 9);
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
	m_CS->IsInWater(m_Camera->GetPosition(), m_PlayerRect) ? SetInWater(true) : SetInWater(false);
	Move(deltaTime);

	m_SpacePressed = false;
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

	// Determen what values to use for physics calculations
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
	m_SpacePressed = true;
	if (m_Flying)
	{
		glm::vec3 nextPos = m_Camera->GetPosition() + glm::vec3(0.f, 1.f, 0.f) * m_MovementSpeed * deltaTime;
		if (!m_CS->CheckGridCollision(nextPos, m_PlayerRect))
			SetPosition(nextPos);
	}
	else if (m_InWater)
	{

		//float jumpBoost = JUMP_SPEED * 0.7f;
		//m_Vel.y = jumpBoost;
		//m_OnGround = false;
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

	float grav = m_InWater ? GRAVITY * 0.1f : GRAVITY;
	float maxFall = m_InWater ? MAX_FALL_SPEED * 0.1f : MAX_FALL_SPEED;

	const float buoyancyStrength = 24.0f; 
	const float damping = 0.95f;          
	const float surfaceSlack = 0.8f; 

	float waterSurfaceY = 66.0f;
	float playerY = m_Pos.y;

	if (m_InWater && m_SpacePressed) {
		float depth = waterSurfaceY - playerY; // positive if below surface
		if (depth > surfaceSlack) {
			// Apply buoyancy proportional to depth
			m_Vel.y += buoyancyStrength * (depth / 5 + surfaceSlack) * deltaTime;
			// Damping for bounce
			m_Vel.y *= damping;
		}
		else {
			// If above surface, let gravity pull down
			m_Vel.y -= grav * deltaTime;
		}
	}
	else {
		// Normal gravity (or slow gravity in water)
		m_Vel.y -= grav * deltaTime;
	}

	// Clamp fall speed
	if (m_Vel.y < -maxFall) m_Vel.y = -maxFall;

	// Move and check collision
	glm::vec3 newPos = m_Camera->GetPosition();
	float changeY = m_Vel.y * deltaTime;
	newPos.y += changeY;

	if (!m_CS->CheckGridCollision(newPos, m_PlayerRect))
	{
		SetPosition(newPos);
		m_OnGround = false;
	}
	else
	{
		m_Vel.y = 0;
		m_OnGround = true;
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

bool Player::IsRunning() const
{
	glm::vec3 velXZ = glm::vec3(m_Vel.x, 0.0f, m_Vel.z);
	if (velXZ != glm::vec3(0))
	{
		float speed = glm::length(velXZ);
		if (speed > MAX_GROUND_SPEED)
			return true;
	}
	return false;
}

bool Player::GetUnderWater() const
{
	glm::vec3 pos = m_Camera->GetPosition();
	pos.y += 1.9f;
	return m_CS->IsInWater(pos, m_PlayerRect);
}

void Player::SetPosition(const glm::vec3& pos)
{
	m_Pos = pos;
	m_Camera->SetPosition(pos);
}

bool Player::CanJump() const
{
	if (m_InWater) return false;
	if (m_Flying) return true;
	if (m_OnGround) return true;
	return false;
}
