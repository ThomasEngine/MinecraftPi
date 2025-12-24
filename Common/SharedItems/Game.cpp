#include "Game.h"
#include "Input.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iostream>
#include "IGraphics.h"

#include "IInput.h"
#include <map>
#include "ICommand.h"

#include <string>

#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "OnBlock.h"
#include "Crosshair.h"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "gui.h"

#ifdef WINDOWS_BUILD
#include "WindowsGraphics.h"
#endif // 

#include "UIManager.h"
#include "Renderer2D.h"

#include "Camera.h"
#include <thread> 

#include "BlockRegistery.h"
#include "World.h"
#include "CollisionSystem.h"
#include "MobFactory.h"


Game::Game(const Input* const input, IGraphics* graphics) :
	input(input),
	graphics(graphics),
	m_Camera(WINDOW_WIDTH, WINDOW_HEIGHT),
	m_InputManager(input)
{

}

Game::~Game()
{

}


void Game::Start()
{
	InitializeOpenGLES();
	printf("This cross project was partly inspired by BUas Student Ferri de Lange\n");
	printf("This GPU supplied by  :%s\n", glGetString(GL_VENDOR));
	printf("This GPU supports GL  :%s\n", glGetString(GL_VERSION));
	printf("This GPU Renders with :%s\n", glGetString(GL_RENDERER));
	printf("This GPU Shaders are  :%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Timing
	auto startTime = std::chrono::system_clock::now();
	auto lastTime = startTime;

	float averageFPS{ 0 };
	float moveSpeed = 5.f;

	
	Renderer renderer;
	if (!renderer.init()) {
		std::cerr << "Renderer init failed\n";
	}

#ifdef WINDOWS_BUILD
	Shader shader("Common/SharedItems/Assets/Basic.shader");
	Texture* testTex = new Texture("Common/SharedItems/Assets/minecraftAtlas.png");
	Shader otherShader("Common/SharedItems/Assets/OnBlock.shader");
	Shader uiShader("Common/SharedItems/ui.shader");
	Texture* uiTex = new Texture("Common/SharedItems/Assets/basicWidget.png");
#endif
#ifdef Raspberry_BUILD
	Shader shader("../Common/SharedItems/Assets/Basic.shader");
	Shader otherShader("../Common/SharedItems/Assets/OnBlock.shader");
	Texture* testTex = new Texture("../Common/SharedItems/Assets/minecraftAtlas.png");
	Shader uiShader("../Common/SharedItems/Assets/ui.shader");
	Texture* uiTex = new Texture("../Common/SharedItems/Assets/basicWidget.png");
#endif


	//Shader shader("Common/SharedItems/Assets/Basic.shader");
	GLuint program = shader.GetID();
	//GLuint program = renderer.createProgramFromSource(vs_src, fs_src);

	if (!program) {
		std::cerr << "Failed to create shader program\n";
		renderer.shutdown();
	}
	//Texture* testTex = new Texture("Common/SharedItems/Assets/MinecraftTex.png");
	//Texture testTex("Common/SharedItems/Assets/dirtblock.png");
	InitializeBlockTypes();

	// Before drawing
	shader.Bind();
	shader.SetUniform1i("u_TextureAtlas", 0);
	shader.SetUniform1f("u_CellWidth", 1.f / 16.f);
	shader.SetUniform1f("u_CellHeight", 1.f / 16.f);
	testTex->Bind(0);

	// Seed random number generator 
	srand(std::time(nullptr));

	// Create world
    m_CollisionSystem = std::make_shared<CollisionSystem>();
	world = new World(renderer, rand(), &m_Camera);
	m_CollisionSystem->SetBlockTarget(*world);
	world->SetCollisionSystem(m_CollisionSystem);

	SetupCommands();
	


	// initialize blocks
	Initialize();

	dayTime = 11.9f; // Noon

	// setup GUI
	gui = new Gui(world, this);
	m_OnBlock = new OnBlock(renderer);
	Crosshair crosshair(renderer);

#ifdef WINDOWS_BUILD
	gui->SetupPc(&graphics->GetWindow());
#else
	gui->SetupPi();
#endif 
	
	// Setup 2D renderer
	Renderer2D uiRenderer;
	uiRenderer.init(uiTex, &uiShader);
	//uiRenderer.init(testTex, &uiShader);
	// Create ui manager

	windowW = graphics->GetWindowWidth();
	windowH = graphics->GetWindowHeight();
	m_UIManager = new UIManager();
    m_UIManager->Initialize(*world->GetPlayer().GetInventory(), *this, uiRenderer);
	m_UIManager->SetWindowSize(windowW, windowH);
	m_UIManager->CloseInventory();
	while(!quitting)
	{
		CheckGameResize();
		windowW = graphics->GetWindowWidth();
		windowH = graphics->GetWindowHeight();
		// sleep to save CPU
		std::this_thread::sleep_for(std::chrono::milliseconds(5));

		auto time = std::chrono::system_clock::now();
		std::chrono::duration<float> delta = time - lastTime;

		gameDeltaTime = delta.count();

		ProcessInput(m_Camera, renderer, gameDeltaTime, moveSpeed);

		std::chrono::duration<float> elapsed = time - startTime;
		if(elapsed.count() > 0.25f && frameCount > 10)
		{
			averageFPS = static_cast<float>(frameCount) / elapsed.count();
			startTime = time;
			frameCount = 0;
		}
		ClearScreen();
		//Update and Draw your game here
	
		// Updates
		Update(gameDeltaTime);
		m_InputManager.Update(gameDeltaTime);
		m_InputManager.ProcessCommands(world->GetPlayer(), gameDeltaTime);
		shader.Bind();
		float timeOfDay = fmod(dayTime / 12.f, 1.f);
		shader.SetUniform1f("u_DayTime", timeOfDay);
		shader.Unbind();
		glm::mat4 projView = m_Camera.GetViewProjectionMatrix();
		world->Update(m_Camera.GetDirection(), m_Camera.GetPosition(), projView, gameDeltaTime);
		m_Camera.Update(gameDeltaTime);
		m_UIManager->Update(gameDeltaTime, *input);
		crosshair.Update(windowW, windowH);
		// Render
		Render();
		float playerUnderwater = world->GetPlayer().GetUnderWater() ? 1.f : 0.f;
		shader.Bind();
		shader.SetUniform1f("u_UnderWater", playerUnderwater);
		

		
		world->Draw(projView, shader, *testTex);
		

		// Gui elements
		m_OnBlock->Render(renderer, m_Camera.GetViewProjectionMatrix(), *testTex, otherShader);
		crosshair.Render(renderer, *testTex);

		uiRenderer.beginFrame(windowW, windowH);
		m_UIManager->Render();
		uiRenderer.endFrame();
		// Post Render
		PostRender();
		if (renderimGUI)
		{
			gui->NewFrame();
			gui->Window(averageFPS, moveSpeed, dayTime, blockToPlace);
			gui->Render();
		}

		graphics->SwapBuffer();
		lastTime = time;
		++frameCount;
	}
	delete testTex;
	graphics->Quit();
	Shutdown();
}


const Input& Game::GetInput() const
{
	return *input;
}

void Game::Quit()
{
	quitting = true;
}


void Game::GetScreenHeightAndWidth(int& width, int& height) const
{
	// Callback to get screen width and height
	width = graphics->GetWindowWidth();
	height = graphics->GetWindowHeight();
}

void Game::CheckGameResize()
{
	// Check for window resize
	int width = graphics->GetWindowWidth();
	int height = graphics->GetWindowHeight();
	if (width != windowW || height != windowH)
	{
		windowW = width;
		windowH = height;
		OnGameResize(width, height);
	}
}

void Game::OnGameResize(int width, int height)
{
	// Gets called by CheckGameResize
	// All resize related stuff
	m_UIManager->SetWindowSize(width, height);
	m_Camera.SetPerspective(glm::radians(70.f), (float)width / (float)height, 0.2f, 256.f);

	// Add new resize stuff here when needed
}

//example of using the key and mouse
void Game::ProcessInput(Camera& cam, Renderer& renderer, float deltaTime, float speed)
{
	Player& m_Player = world->GetPlayer();
	const Input& input = GetInput();
	const IMouse& mouse = input.GetMouse();
	const IKeyboard& keyboard = input.GetKeyboard();
	float moveSpeed = speed;
	float lookSpeed = 0.008;
	static bool toggledCurser = false;

	ProcessInventoryCommands();

	glm::vec2 currentMouse = mouse.GetPosition();
	glm::vec2 delta = currentMouse - lastMouse;
	lastMouse = currentMouse;

	if (!toggledCurser)
	{
		cam.AddYaw(-delta.x * lookSpeed);
		cam.AddPitch(-delta.y * lookSpeed);
	}
	
	// Inventory stuff


	if (keyboard.GetKey(Key::CTRL_LEFT))
	{
		m_Player.SetSprinting(true);
		m_Player.GetCamera()->SetSprintFov(m_Player.IsRunning() ? true : false);
	}
	else { 
		m_Player.SetSprinting(false);
		m_Player.GetCamera()->SetSprintFov(false);
	}

	if (keyboard.GetKey(Key::SHIFT_LEFT))
		m_Player.SetChrouching(true);
	else m_Player.SetChrouching(false);
	if (speedBoost)
		moveSpeed = 8;

	m_Player.SetMoveSpeed(moveSpeed);

	if (keyboard.GetKey(Key::ESCAPE))
		Quit();

#ifdef WINDOWS_BUILD
	if (keyboard.GetKey(Key::ALT_LEFT) && canBreakBlock)
	{
		toggledCurser = !toggledCurser;
		dynamic_cast<WindowsGraphics*>(graphics)->ToggleCurser();
		canBreakBlock = false;
		blockTimer = 0;
		renderimGUI = !renderimGUI;
	}
#endif 

	if (keyboard.GetKey(Key::ARROW_LEFT))
		cam.AddYaw(lookSpeed * 10.0f);
	if (keyboard.GetKey(Key::ARROW_RIGHT))
		cam.AddYaw(-lookSpeed * 10.0f);
	if (keyboard.GetKey(Key::ARROW_UP))
		cam.AddPitch(lookSpeed * 10.0f);
	if (keyboard.GetKey(Key::ARROW_DOWN))
		cam.AddPitch(-lookSpeed * 10.0f);


	if (!canBreakBlock)
		blockTimer += deltaTime;
	if (blockTimer >= .2f)
		canBreakBlock = true;
	glm::ivec3 blockPosRayOutline{ -1, -1, -1 };
	// Ray
	{
		glm::vec3 camPos = cam.GetPosition();
		camPos.y += .8f; // m_Camera height offset
		glm::vec3 camDir = cam.GetDirection();
		float maxDistance = 5.0f;
		float step = 0.2f;
		for (float i = 0; i < maxDistance; i += step)
		{
			glm::vec3 pos = camPos + camDir * i;
			int blockX = int(floor(pos.x));
			int blockY = int(floor(pos.y));
			int blockZ = int(floor(pos.z));
			if (g_BlockTypes[world->GetBlockAtPosition(glm::vec3(blockX, blockY, blockZ))].isSolid) 
			{
				blockPosRayOutline = glm::ivec3(blockX, blockY, blockZ);
				m_OnBlock->Update(gameDeltaTime, blockPosRayOutline);
				if (mouse.GetButtonDown(MouseButtons::LEFT) && canBreakBlock)
				{
					world->RemoveBlockAtPosition(glm::vec3(blockX, blockY, blockZ));
					canBreakBlock = false;
					blockTimer = 0;
				}
				break;
			}
			else
			{
				blockPosRayOutline = glm::ivec3(-1, -1, -1);
				m_OnBlock->Update(gameDeltaTime, glm::ivec3(-1));
			}
		}
	}
	printf("Block Position Outline: [%d, %d, %d]       \r", blockPosRayOutline.x, blockPosRayOutline.y, blockPosRayOutline.z);

	// Place blocks
	if (mouse.GetButtonDown(MouseButtons::RIGHT) && canBreakBlock)
	{
		glm::vec3 camPos = cam.GetPosition();
		camPos.y += .8f; // m_Camera height offset
		glm::vec3 camDir = cam.GetDirection();
		float maxDistance = 5.0f;
		float step = 0.05f;
		glm::vec3 lastAirBlock = camPos;
		for (float i = 0; i < maxDistance; i += step)
		{
			glm::vec3 pos = camPos + camDir * i;
			int blockX = int(floor(pos.x));
			int blockY = int(floor(pos.y));
			int blockZ = int(floor(pos.z));
			if (g_BlockTypes[world->GetBlockAtPosition(glm::vec3(blockX, blockY, blockZ))].isSolid)
			{
				if (m_CollisionSystem->CehckPlayerToBlock(m_Camera.GetPosition(), lastAirBlock, m_Player.getRect()))
				{
					break;
				}
				world->PlaceBlockAtPosition(lastAirBlock, blockToPlace); 
				canBreakBlock = false;
				blockTimer = 0;
				break;
			}
			lastAirBlock = glm::vec3(blockX, blockY, blockZ);
		}
	}
}

void Game::InitializeOpenGLES()
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRangef(0.0f, 1.0f);
	glClearDepthf(1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::ClearScreen()
{
	float timeOfDay = fmod(dayTime / 12.f, 1.f);
	glClearColor(0.53f * timeOfDay, 0.81f * timeOfDay, 0.92f * timeOfDay, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Game::SetupCommands()
{
	// Setup input commands
	// BindCommand 
	m_InputManager.BindCommand("MoveForward", std::make_unique<MoveForwardCommand>());
	m_InputManager.BindCommand("MoveBackward", std::make_unique<MoveBackwardCommand>());
	m_InputManager.BindCommand("MoveLeft", std::make_unique<MoveLeftCommand>());
	m_InputManager.BindCommand("MoveRight", std::make_unique<MoveRightCommand>());
	m_InputManager.BindCommand("Crouch", std::make_unique<CrouchCommand>());
	m_InputManager.BindCommand("Jump", std::make_unique<JumpCommand>());

	// TODO:: ADD sprint command

	// Bind keys to actions
	m_InputManager.BindAction(Key::W, "MoveForward");
	m_InputManager.BindAction(Key::S, "MoveBackward");
	m_InputManager.BindAction(Key::A, "MoveLeft");
	m_InputManager.BindAction(Key::D, "MoveRight");
	m_InputManager.BindAction(Key::SHIFT_LEFT, "Crouch");
	m_InputManager.BindAction(Key::SPACE, "Jump");
	// Direct key to command map for simpler access in ProcessInput

	// Inventory toggle
	m_InputManager.BindAction(Key::E, "ToggleInventory");
	m_InputManager.BindAction(MouseButtons::SCROLL_DOWN, "Inventory-");
	m_InputManager.BindAction(MouseButtons::SCROLL_UP, "Inventory+");
	// Inventory 1 - 9
	m_InputManager.BindAction(Key::NUM_1, "InventoryIndexOne");
	m_InputManager.BindAction(Key::NUM_2, "InventoryIndexTwo");
	m_InputManager.BindAction(Key::NUM_3, "InventoryIndexThree");
	m_InputManager.BindAction(Key::NUM_4, "InventoryIndexFour");
	m_InputManager.BindAction(Key::NUM_5, "InventoryIndexFive");
	m_InputManager.BindAction(Key::NUM_6, "InventoryIndexSix");
	m_InputManager.BindAction(Key::NUM_7, "InventoryIndexSeven");
	m_InputManager.BindAction(Key::NUM_8, "InventoryIndexEight");
	m_InputManager.BindAction(Key::NUM_9, "InventoryIndexNine");
}

void Game::ProcessInventoryCommands()
{
	// Open and close inventory
	if (m_InputManager.IsKeyboardActionActive("ToggleInventory"))
	{
		m_UIManager->ToggleInventory();
	}

	// Scrolling
	if (m_InputManager.IsMouseActionActive("Inventory+"))
	{
		m_UIManager->HotBarIndexUp();
	}
	if (m_InputManager.IsMouseActionActive("Inventory-"))
	{
		m_UIManager->HotBarDown();
	}

	// Hotbar selection 1-9
	if (m_InputManager.IsKeyboardActionActive("InventoryIndexOne"))
	{
		m_UIManager->SetHotBarIndex(0);
	}
	if (m_InputManager.IsKeyboardActionActive("InventoryIndexTwo"))
	{
		m_UIManager->SetHotBarIndex(1);
	}
	if (m_InputManager.IsKeyboardActionActive("InventoryIndexThree"))
	{
		m_UIManager->SetHotBarIndex(2);
	}
	if (m_InputManager.IsKeyboardActionActive("InventoryIndexFour"))
	{
		m_UIManager->SetHotBarIndex(3);
	}
	if (m_InputManager.IsKeyboardActionActive("InventoryIndexFive"))
	{
		m_UIManager->SetHotBarIndex(4);
	}
	if (m_InputManager.IsKeyboardActionActive("InventoryIndexSix"))
	{
		m_UIManager->SetHotBarIndex(5);
	}
	if (m_InputManager.IsKeyboardActionActive("InventoryIndexSeven"))
	{
		m_UIManager->SetHotBarIndex(6);
	}
	if (m_InputManager.IsKeyboardActionActive("InventoryIndexEight"))
	{
		m_UIManager->SetHotBarIndex(7);
	}
	if (m_InputManager.IsKeyboardActionActive("InventoryIndexNine"))
	{
		m_UIManager->SetHotBarIndex(8);
	}
}
