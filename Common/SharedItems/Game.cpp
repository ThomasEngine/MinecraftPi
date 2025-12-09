#include "Game.h"
#include "Input.h"
#include <chrono>
#include <ctime>
#include <iomanip>
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

#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "gui.h"
//#include "WindowsGraphics.h"

#include "Camera.h"

#include "BlockRegistery.h"
#include "World.h"
#include "CollisionSystem.h"
#include "MobFactory.h"


Game::Game(const Input* const input, IGraphics* graphics/*, Gui* mGui*/) :
	input(input),
	graphics(graphics),
	m_Camera(WINDOW_WIDTH, WINDOW_HEIGHT),
	//gui(mGui),
	m_Player(WINDOW_WIDTH, WINDOW_HEIGHT, &m_Camera)
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
#endif
#ifdef Raspberry_BUILD
	Shader shader("../Common/SharedItems/Assets/basicpi.shader");
	Texture* testTex = new Texture("../Common/SharedItems/Assets/minecraftAtlas.png");
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

	world = new World(renderer);
	collisionSystem = new CollisionSystem();
	collisionSystem->SetBlockTarget(*world);
	m_Player.SetCollisionSystem(collisionSystem);


	keyCommandMap[Key::W] = std::make_unique<MoveForwardCommand>();
	keyCommandMap[Key::S] = std::make_unique<MoveBackwardCommand>();
	keyCommandMap[Key::A] = std::make_unique<MoveLeftCommand>();
	keyCommandMap[Key::D] = std::make_unique<MoveRightCommand>();
	keyCommandMap[Key::SHIFT_LEFT] = std::make_unique<CrouchCommand>();
	keyCommandMap[Key::SPACE] = std::make_unique<JumpCommand>();

	Initialize();
	mobFactory = new MobFactory(renderer);
	std::vector<Mob*> mobs;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			Mob* sheepPrototype = mobFactory->create("Sheep", { 0, 170, 0 });
			sheepPrototype->setPosition(glm::vec3(-j, 95, i));
			sheepPrototype->SetCollisionSystem(collisionSystem);
			mobs.push_back(sheepPrototype);
		}
	}


	dayTime = 11.9f; // Noon

	gui = new Gui(&m_Player, world);
	

#ifdef WINDOWS_BUILD
	gui->SetupPc(&graphics->GetWindow());
#else
	gui->SetupPi();
#endif 

	while(!quitting)
	{
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
		shader.Bind();
		float timeOfDay = fmod(dayTime / 12.f, 1.f);
		shader.SetUniform1f("u_DayTime", timeOfDay);
		shader.Unbind();
		glm::mat4 projView = m_Camera.GetViewProjectionMatrix();
		world->Update(m_Camera.GetDirection(), m_Camera.GetPosition(), projView);
		m_Player.Update(gameDeltaTime);
		// Render
		Render();
		
		world->Draw(projView, shader, *testTex);
			
		for (auto& mob : mobs)
		{
			mob->update(gameDeltaTime, m_Player.GetCamera()->GetPosition());
			mob->render(renderer, shader, *testTex, m_Camera.GetViewProjectionMatrix());
		}



		// Post Render
		PostRender();
		gui->NewFrame();
		gui->Window(averageFPS, moveSpeed, dayTime);
		gui->Render();
		//printf("Avera/e FPS: %.2f\r", averageFPS);

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


//example of using the key and mouse
void Game::ProcessInput(Camera& cam, Renderer& renderer, float deltaTime, float speed)
{
	const Input& input = GetInput();
	const IMouse& mouse = input.GetMouse();
	const IKeyboard& keyboard = input.GetKeyboard();
	float moveSpeed = speed;
	float lookSpeed = 1.12f * deltaTime;

	static glm::vec2 lastMouse = mouse.GetPosition();
	glm::vec2 currentMouse = mouse.GetPosition();
	glm::vec2 delta = currentMouse - lastMouse;
	lastMouse = currentMouse;

	cam.AddYaw(-delta.x * lookSpeed);
	cam.AddPitch(-delta.y * lookSpeed);

	if (keyboard.GetKey(Key::CTRL_LEFT))
		speedBoost = true;
	else speedBoost = false;

	if (speedBoost)
		moveSpeed = 8;

	m_Player.SetMoveSpeed(moveSpeed);
	for (const auto& pair : keyCommandMap) {
		if (keyboard.GetKey(pair.first)) {
			pair.second->Execute(m_Player, gameDeltaTime);
		}
	}

	if (keyboard.GetKey(Key::ESCAPE))
		Quit();

#ifdef WINDOWS_BUILD
	if (keyboard.GetKey(Key::ALT_LEFT))
	{
		//dynamic_cast<WindowsGraphics*>(graphics)->ToggleCurser();
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

	// Break blocks
	if (mouse.GetButtonDown(MouseButtons::LEFT) && canBreakBlock)
	{
		glm::vec3 camPos = cam.GetPosition();
		camPos.y += .95f; // m_Camera height offset
		glm::vec3 camDir = cam.GetDirection();
		float maxDistance = 5.0f;
		float step = 1.f;
		for (float i = 0; i < maxDistance; i += step)
		{
			glm::vec3 pos = camPos + camDir * i;
			int blockX = int(floor(pos.x));
			int blockY = int(floor(pos.y));
			int blockZ = int(floor(pos.z));
			if (world->GetBlockAtPosition(glm::vec3(blockX, blockY, blockZ)) != 0) // 0 == air
			{
				world->RemoveBlockAtPosition(glm::vec3(blockX, blockY, blockZ));
				canBreakBlock = false;
				blockTimer = 0;
				break;
			}
		}
	}

	// Place blocks
	if (mouse.GetButtonDown(MouseButtons::RIGHT) && canBreakBlock)
	{
		glm::vec3 camPos = cam.GetPosition();
		camPos.y += .9f; // m_Camera height offset
		glm::vec3 camDir = cam.GetDirection();
		float maxDistance = 5.0f;
		float step = 0.1f;
		glm::vec3 lastAirBlock = camPos;
		for (float i = 0; i < maxDistance; i += step)
		{
			glm::vec3 pos = camPos + camDir * i;
			int blockX = int(floor(pos.x));
			int blockY = int(floor(pos.y));
			int blockZ = int(floor(pos.z));
			if (world->GetBlockAtPosition(glm::vec3(blockX, blockY, blockZ)) != B_AIR)
			{
				if (collisionSystem->CehckPlayerToBlock(m_Camera.GetPosition(), lastAirBlock, m_Player.getRect()))
				{
					break;
				}
				world->PlaceBlockAtPosition(lastAirBlock, 2); 
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
