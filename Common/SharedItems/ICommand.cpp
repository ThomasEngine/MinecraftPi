#include "ICommand.h"
#include "Player.h"
#include "Camera.h"

void MoveForwardCommand::Execute(Player& player, float dt)
{
	player.MoveForward(dt);
}

void MoveRightCommand::Execute(Player& player, float dt)
{
	player.MoveRight(dt);
}

void JumpCommand::Execute(Player& player, float dt)
{
	player.Jump(dt);
}

void CrouchCommand::Execute(Player& player, float dt)
{
	player.Crouch();
}

void StartFlyingCommand::Execute(Player& player, float dt)
{
	player.SetFlying(true);
}

void StopFlyingCommand::Execute(Player& player, float dt)
{
	player.SetFlying(false);
}

void MoveBackwardCommand::Execute(Player& player, float dt)
{
	player.MoveBackward(dt);
}

void MoveLeftCommand::Execute(Player& player, float dt)
{
	player.MoveLeft(dt);
}
