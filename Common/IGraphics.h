// a simple interface class its basically just simple access to graphics
#pragma once
class IGraphics
{
public:
	virtual ~IGraphics() = default;
	virtual void SwapBuffer() = 0;
	virtual void Quit() = 0;
	virtual int GetWindowWidth() const = 0;
	virtual int GetWindowHeight() const = 0;
	virtual void ToggleCurser() const = 0;

#ifdef WINDOWS_BUILD
	virtual GLFWwindow& GetWindow() = 0;
#endif 

};

