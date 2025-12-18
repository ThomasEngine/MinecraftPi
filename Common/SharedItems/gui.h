#pragma once
#ifdef WINDOWS_BUILD
//include glad and glfw for Windows build
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#else
#include <GLES2/gl2.h>
#endif

#ifdef Raspberry_BUILD
#include <GLES3/gl3.h>
#endif

#include <glm/glm.hpp>

class Player;
class World;
class Gui
{
public:
    Gui(Player* p, World* world);
    ~Gui();
#ifdef WINDOWS_BUILD
    void SetupPc(GLFWwindow* window);
#endif
    void SetupPi();
    void NewFrame();
    void Window(float& FPS, float& movespeed, float& daytime, uint8_t& block);
	void AddTextWithData(const char* text, float&); 
    void Render();
    void ShutDown();

private:
	Player* player;
    World* world;
};