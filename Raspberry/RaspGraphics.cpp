#include "RaspGraphics.h"
#include <EGL/egl.h>
#include "EGLState.h"
#include "RaspMouse.h"
#include "XWindow.h"

RaspGraphics::RaspGraphics() : window(new XWindow())
{
	window->CreateWindow();
}

void RaspGraphics::Quit()
{
	XDestroyWindow(&window->GetDisplay(), window->GetWindow());
}

void RaspGraphics::SwapBuffer()
{
	EGLState state = window->GetState();
	eglSwapBuffers(state.display, state.surface);
}

int RaspGraphics::GetWindowWidth() const
{
	int height;
	int width;
	EGLState state = window->GetState();
	eglQuerySurface(state.display, state.surface, &width, &height);
	return width;
}

int RaspGraphics::GetWindowHeight() const
{
	int height;
	int width;
	EGLState state = window->GetState();
	eglQuerySurface(state.display, state.surface, &width, &height);
	return height;
}

XWindow& RaspGraphics::Window() const
{
	return *window;
}
