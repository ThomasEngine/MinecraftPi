#pragma once
#include "../IGraphics.h"

class XWindow;
struct EGLState;

class RaspGraphics : public IGraphics
{
public:
	RaspGraphics();
	~RaspGraphics() {};

	void Quit() override;

	void SwapBuffer() override;
	int GetWindowWidth() const override;
	int GetWindowHeight() const override;
	void ToggleCurser() const override;


	XWindow& Window() const;

private:
	XWindow* window;
};

