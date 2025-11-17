#include "Game.h"
#include "Input.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "IGraphics.h"
#include "IInput.h"

#include <string>

#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "gui.h"

#include "Cube.h"
#include "Camera.h"

#include "Chunk.h"
#include "BlockRegistery.h"
#include "ChunkManager.h"


Game::Game(const Input* const input, IGraphics* graphics, Gui* mGui) :
	input(input),
	graphics(graphics),
	gui(mGui)
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

	
	Renderer renderer;
	if (!renderer.init()) {
		std::cerr << "Renderer init failed\n";
	}

#ifdef WINDOWS_BUILD
	Shader shader("Common/SharedItems/Assets/Basic.shader");
	Texture* testTex = new Texture("Common/SharedItems/Assets/MinecraftTex.png");
#endif
#ifdef Raspberry_BUILD
	Shader shader("../Common/SharedItems/Assets/basicpi.shader");
	Texture* testTex = new Texture("../Common/SharedItems/Assets/MinecraftTex.png");
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

	ChunkManager chunkManager(renderer);

	Camera cam(WINDOW_WIDTH, WINDOW_HEIGHT);
	cam.SetPosition(glm::vec3(0, 66, 0));

	while(!quitting)
	{
		auto time = std::chrono::system_clock::now();
		std::chrono::duration<float> delta = time - lastTime;

		gameDeltaTime = delta.count();

		ProcessInput(cam, renderer, gameDeltaTime);

		std::chrono::duration<float> elapsed = time - startTime;
		if(elapsed.count() > 0.25f && frameCount > 10)
		{
			averageFPS = static_cast<float>(frameCount) / elapsed.count();
			startTime = time;
			frameCount = 0;
		}
		// Setup the viewport
		ClearScreen();


		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		//Update and Draw your game here
		renderer.beginFrame();
	

		// Updates
		glm::mat4 projView = cam.GetViewProjectionMatrix();
		chunkManager.Update(gameDeltaTime, cam.GetPosition(), cam.GetDirection(), renderer);

		// Draw
		chunkManager.Draw(renderer, projView, shader, *testTex);

		gui->newFrame();
		{
			{
				ImGui::Begin("Window");

				ImGui::Text("FPS: %f", averageFPS);
				ImGui::Text("--Pos--");
				glm::vec3 camPos = cam.GetPosition();
				ImGui::Text("X: %f", camPos.x);
				ImGui::Text("Y: %f", camPos.y);
				ImGui::Text("Z: %f", camPos.z);

				ImGui::End();
			}
		}
		gui->render();

		graphics->SwapBuffer();
		lastTime = time;
		++frameCount;
	}
	delete testTex;
	graphics->Quit();
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
void Game::ProcessInput(Camera& cam, Renderer& renderer/*, Chunk& chunk*/, float deltaTime)
{
	const Input& input = GetInput();
	const IMouse& mouse = input.GetMouse();
	const IKeyboard& keyboard = input.GetKeyboard();
	float moveSpeed = 8.f * gameDeltaTime;
	float lookSpeed = 1.12f * gameDeltaTime;

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
		moveSpeed *= 3;

	if (keyboard.GetKey(Key::W))
		cam.MoveForward(moveSpeed);
	if (keyboard.GetKey(Key::S))
		cam.MoveForward(-moveSpeed);
	if (keyboard.GetKey(Key::A))
		cam.MoveRight(moveSpeed);
	if (keyboard.GetKey(Key::D))
		cam.MoveRight(-moveSpeed);
	if (keyboard.GetKey(Key::SPACE))
		cam.Move(glm::vec3(0, moveSpeed, 0));
	if (keyboard.GetKey(Key::SHIFT_LEFT))
		cam.Move(glm::vec3(0, -moveSpeed, 0));

	if (keyboard.GetKey(Key::ESCAPE))
		Quit();



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
	//if ((mouse.GetButtonDown(MouseButtons::LEFT) || mouse.GetButtonDown(MouseButtons::RIGHT)) && canBreakBlock)
	//{
	//	glm::vec3 camPos = cam.GetPosition();
	//	glm::vec3 camDir = cam.GetDirection();

	//	float maxDistance = 5.0f;
	//	float step = 1.f;
	//	glm::vec3 lastBlock = camPos;
	//	for (float i = 0; i < maxDistance; i += step)
	//	{
	//		glm::vec3 pos = camPos + camDir * i;
	//		int blockX = static_cast<int>(floor(pos.x));
	//		int blockY = static_cast<int>(floor(pos.y));
	//		int blockZ = static_cast<int>(floor(pos.z));

	//		if (chunk.GetBlock(blockX, blockY, blockZ) != 0) // 0 == air
	//		{
	//			if (mouse.GetButtonDown(MouseButtons::LEFT))
	//			{
	//				chunk.SetBlock(blockX, blockY, blockZ, 0);
	//			}
	//			if (mouse.GetButtonDown(MouseButtons::RIGHT))
	//			{
	//				if (lastBlock != camPos)
	//					chunk.SetBlock(lastBlock.x, lastBlock.y, lastBlock.z, 2);
	//			}

	//			chunk.createChunkMesh(renderer);

	//			canBreakBlock = false;
	//			blockTimer = 0;
	//			break;
	//		}
	//	lastBlock = { blockX, blockY, blockZ };
	//	
	//	}
	//}
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

	glCullFace(GL_BACK);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void Game::ClearScreen()
{
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
