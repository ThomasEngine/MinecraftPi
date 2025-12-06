#pragma once
#include <glm/vec2.hpp>
#include <vector>
#include <map>
#include <memory>
#include "Player.h"
#include "Camera.h"


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
class ICommand;
class CollisionSystem;
class World;
enum class Key;

constexpr unsigned int WINDOW_WIDTH = 1920;
constexpr unsigned int WINDOW_HEIGHT = 1080;
constexpr float ASPECT_RATIO = WINDOW_WIDTH / WINDOW_HEIGHT;

class Game
{
public:
	Game(const Input* const input, IGraphics* graphics/*, Gui* mGui*/);
	virtual ~Game();
	void Start(); 
	const Input& GetInput() const;
	void Quit();
protected:
	void ProcessInput(Camera& cam, Renderer& renderer/*, Chunk& currentChunk*/, float deltaTime, float moveSpeed);
	virtual void Initialize() {}
	virtual void Shutdown() {}
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

	Camera m_Camera;
	World* world;
	Player m_Player;
	float dayTime;
	std::map<Key, std::unique_ptr<ICommand>> keyCommandMap;
	CollisionSystem* collisionSystem = nullptr;
	bool speedBoost;
	bool canBreakBlock = true;
	float blockTimer = 0;
	int frameCount{0};
};

