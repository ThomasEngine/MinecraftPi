#pragma once
#include <glm/vec2.hpp>
#include <vector>

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

class IGraphics;
class Input;
class Renderer;
class Camera;
class Chunk;
class Gui;

constexpr unsigned int WINDOW_WIDTH = 1024;
constexpr unsigned int WINDOW_HEIGHT = 768;
constexpr float ASPECT_RATIO = 1024.0f / 768.0f;

class Game
{
public:
	Game(const Input* const input, IGraphics* graphics, Gui* mGui);
	virtual ~Game();
	void Start(); 
	const Input& GetInput() const;
	void Quit();
protected:
	void ProcessInput(Camera& cam, Chunk& chunk, Renderer& renderer, float deltaTime);
	virtual void Update(float /*gameDeltaTime*/) {}
	virtual void Render() {}
	virtual void PostRender() {}
	
	const Input* const input;
	bool quitting{false};
	float gameDeltaTime;

	IGraphics* graphics;
	Gui* gui = nullptr;

private:
	void InitializeOpenGLES();
	void ClearScreen();



	bool canBreakBlock = true;
	float blockTimer = 0;
	int frameCount{0};
};

