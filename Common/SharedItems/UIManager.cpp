#include "UIManager.h"
#include "InventoryScreen.h"
#include "Input.h"
#include "Game.h"
#include "Renderer2D.h"
#include "PlayerGameInv.h"
#include "IInput.h"

void UIManager::Initialize(Container& playerInv,  Game& game, Renderer2D& ren)
{
	m_Owner = &game;
	m_Renderer2D = &ren;
	m_InventoryScreen = std::make_unique<InventoryScreen>(&playerInv);
	m_PlayerGameInv = std::make_unique<PlayerGameInv>(&playerInv);
	m_PlayerGameInv->onOpen(); // Open hotbar on game start
}

void UIManager::Shutdown()
{
	// Clean up UI screens
}

void UIManager::Update(float deltaTime, const Input& input)
{
	// Update current UI screen if any
	const IKeyboard& keyboard = input.GetKeyboard();
	
	m_InventoryScreen->Update(&input, deltaTime);
	m_PlayerGameInv->Update(&input, deltaTime);

}

void UIManager::Render()
{
	m_InventoryScreen->Render(*m_Renderer2D);
	m_PlayerGameInv->Render(*m_Renderer2D);
}

void UIManager::SetWindowSize(int width, int height)
{
	// Update window size for UI scaling if needed
	m_InventoryScreen->setScreenWidthAndHeight(width, height);
	m_PlayerGameInv->setScreenWidthAndHeight(width, height);
	if (m_InventoryScreen->isOpen())
	{
		m_InventoryScreen->onOpen(); // Re-open to adjust layout
	}
	if (m_PlayerGameInv->isOpen())
	{
		m_PlayerGameInv->onOpen(); // Re-open to adjust layout
	}
}

void UIManager::OpenInventory()
{
	m_InventoryScreen->onOpen();
	m_PlayerGameInv->onClose();
}

void UIManager::CloseInventory()
{
	m_InventoryScreen->onClose();
	m_PlayerGameInv->onOpen();
}

void UIManager::ToggleInventory()
{
	if (m_InventoryScreen->isOpen())
	{
		CloseInventory();
	}
	else OpenInventory();
}
