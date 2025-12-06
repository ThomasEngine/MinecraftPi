#pragma once

class Player;
class ICommand
{
public:
	virtual ~ICommand() {}
	virtual void Execute(Player& player, float dt) = 0;
};

class MoveForwardCommand : public ICommand
{
public:
	void Execute(Player& player, float dt) override;
};

class MoveBackwardCommand : public ICommand
{
public:
	void Execute(Player& player, float dt) override;
};

class MoveRightCommand : public ICommand
{
public:
	void Execute(Player& player, float dt) override;
};

class MoveLeftCommand : public ICommand
{
public:
	void Execute(Player& player, float dt) override;
};

class JumpCommand : public ICommand
{
public:
	void Execute(Player& player, float dt) override;
};

class CrouchCommand : public ICommand
{
public:
	void Execute(Player& player, float dt) override;
};

class StartFlyingCommand : public ICommand
{
public:
	void Execute(Player& player, float dt) override;
};

class StopFlyingCommand : public ICommand
{
public:
	void Execute(Player& player, float dt) override;
};
