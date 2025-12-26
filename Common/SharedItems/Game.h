#pragma once
#include <glm/vec2.hpp>
#include <vector>
#include <map>
#include <memory>
#include "Player.h"
#include "Camera.h"
#include "OnBlock.h"
#include "InputManager.h"


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
class MobFactory;
class UIManager;
enum class Key;

constexpr unsigned int WINDOW_WIDTH = 854;
constexpr unsigned int WINDOW_HEIGHT = 480;
constexpr float ASPECT_RATIO = WINDOW_WIDTH / WINDOW_HEIGHT;

class Game
{
public:
	Game(const Input* const input, IGraphics* graphics/*, Gui* mGui*/);
	virtual ~Game();
	void Start(); 
	const Input& GetInput() const;
	void Quit();

	// Helper for now
	void GetScreenHeightAndWidth(int& width, int& height) const;

	void CheckGameResize();
	void OnGameResize(int width, int height);
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
	void SetupCommands();
	void ProcessInventoryCommands();

	void ToggleMouse();
	bool toggledCurser = false;


	UIManager* m_UIManager = nullptr;
	glm::vec2 lastMouse = { 0, 0 };
	InputManager m_InputManager;
	Camera m_Camera;
	OnBlock* m_OnBlock;
	World* world;
	MobFactory* mobFactory = nullptr;
	float dayTime;
	std::shared_ptr<CollisionSystem> m_CollisionSystem;
	int windowW, windowH;
	bool speedBoost;
	bool canBreakBlock = true;
	bool renderimGUI = false;
	uint8_t blockToPlace = 1;
	float blockTimer = 0;
	int frameCount{0};
};

