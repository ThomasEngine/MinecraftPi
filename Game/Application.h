#include "Game.h"
#pragma once


class Application : public Game
{
public:
    Application(const Input* const input, IGraphics* graphics, Gui* gui)
        : Game(input, graphics, gui)
    {
    }

    virtual ~Application() override = default;
    
	void Initialize() override;
	void Shutdown() override;

    void Update(float gameDeltaTime) override;
    void Render() override;
    void PostRender() override;
};

