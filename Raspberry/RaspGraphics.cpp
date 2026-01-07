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
   EGLint width;  
   EGLint height;  
   EGLState state = window->GetState();  
   eglQuerySurface(state.display, state.surface, EGL_WIDTH, &width);  
   return width;  
}  

int RaspGraphics::GetWindowHeight() const  
{  
   EGLint width;  
   EGLint height;  
   EGLState state = window->GetState();  
   eglQuerySurface(state.display, state.surface, EGL_HEIGHT, &height);  
   return height;  
}

void RaspGraphics::ToggleCurser() const
{

}

XWindow& RaspGraphics::Window() const
{
	return *window;
}
