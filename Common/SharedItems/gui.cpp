#include "gui.h"

#include "Player.h"
#include "BlockRegistery.h"
#include "World.h"
#include "Camera.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#ifdef WINDOWS_BUILD
#include <imgui/imgui_impl_glfw.h>
#endif
#include <string>
#include "Game.h"

static bool flying = false;


Gui::Gui(World* world, Game* game)
	: player(&world->GetPlayer()), world(world), owner(game)
{

}

Gui::~Gui()
{

}

#ifdef WINDOWS_BUILD
void Gui::SetupPc(GLFWwindow* window)
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplOpenGL3_Init("#version 100");
}
#endif
void Gui::SetupPi()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.DisplaySize = ImVec2(1920, 1080);

    // make the raspberry a bit more rounded
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.WindowRounding = 4.0f;

    ImGui_ImplOpenGL3_Init("#version 100");
}

void Gui::NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
#ifdef WINDOWS_BUILD
    ImGui_ImplGlfw_NewFrame();
#endif
    ImGui::NewFrame();
}

void Gui::Window(float& FPS, float& moveSpeed, float& dayTime, uint8_t& block)
{
	glm::vec3 camPos = player->GetCamera()->GetPosition();
	ImGui::Begin("Debug");


	ImGui::Checkbox("Flying", &flying);
	player->SetFlying(flying);

	ImGui::Text("FPS: %.2f", FPS);
	ImGui::Text("--Pos--");
	glm::vec3 playerPos = camPos;
	ImGui::Text("X: %f", playerPos.x);
	ImGui::Text("Y: %f", playerPos.y);
	ImGui::Text("Z: %f", playerPos.z);

	// Chunk position
	glm::ivec3 chunkPos = world->WorldToChunkPos(playerPos);
	ImGui::Text("--Chunk Pos--");
	ImGui::Text("X: %d", chunkPos.x);
	ImGui::Text("Y: %d", chunkPos.y);
	ImGui::Text("Z: %d", chunkPos.z);

	// Position in chunk
	glm::ivec3 localPos = glm::ivec3(
		(int)(floor(playerPos.x)) - chunkPos.x * 16,
		(int)(floor(playerPos.y)),
		(int)(floor(playerPos.z)) - chunkPos.z * 16
	);
	ImGui::Text("--Local Pos--");
	ImGui::Text("X: %d", localPos.x);
	ImGui::Text("Y: %d", localPos.y);
	ImGui::Text("Z: %d", localPos.z);

	ImGui::SliderFloat("Move Speed", &moveSpeed, 6.f, 42.f);
	ImGui::SliderFloat("Time", &dayTime, 0.f, 11.9f);

	ImGui::Text("--Block to place--");
	const char* listBlocks[] = {
		g_BlockTypes[B_DIRT].name,
		g_BlockTypes[B_GRASS].name,
		g_BlockTypes[B_STONE].name,
		g_BlockTypes[B_COBBLESTONE].name,
		g_BlockTypes[B_OAK_LOG].name,
		g_BlockTypes[B_OAK_PLANK].name,
		g_BlockTypes[B_OAK_LEAF].name,
		g_BlockTypes[B_BEDROCK].name,
		g_BlockTypes[B_SAND].name,
		g_BlockTypes[B_GRAVEL].name,
		g_BlockTypes[B_WATER].name,
		g_BlockTypes[B_GRANITE].name,
		g_BlockTypes[B_DIORITE].name,
		g_BlockTypes[B_GLOWSTONE].name,
	};

	static int currentBlock = block + 1;
	if (ImGui::ListBox("##Blocks", &currentBlock, listBlocks, IM_ARRAYSIZE(listBlocks), 4))
	{
		block = currentBlock + 1;
	}

	// Ask a change of ratio when changing the screen height and width
	// When press button change aspect ratio to current screen size
    if (ImGui::Button("ratio")) {
		int width, height;
		owner->GetScreenHeightAndWidth(width, height);
		player->GetCamera()->SetPerspective(glm::radians(70.f), (float)width / (float)height, 0.2f, 256.f);
    }

	ImGui::End();
}

void Gui::AddTextWithData(const char* text, float&)
{
}

void Gui::Render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
}

void Gui::ShutDown()
{
    ImGui_ImplOpenGL3_Shutdown();
#ifdef WINDOWS_BUILD
    ImGui_ImplGlfw_Shutdown();
#endif
    ImGui::DestroyContext();
}