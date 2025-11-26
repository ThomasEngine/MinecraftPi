#pragma once
#include <GLFW/glfw3.h>

class Gui
{
public:
	Gui(GLFWwindow* window);
	~Gui();
	void newFrame();
	void render();
	void shutDown();
};