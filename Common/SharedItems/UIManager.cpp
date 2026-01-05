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

	DraggedItem& draggedItem = m_InventoryScreen->draggedItem;
	if (draggedItem.isDragging)
	{
		if (input.GetMouse().GetButtonDown(MouseButtons::LEFT))
		{
			glm::vec2 mousePos = input.GetMouse().GetPosition();
			draggedItem.mousePos = mousePos;
			draggedItem.itemBounds.x = static_cast<int>(mousePos.x + draggedItem.offset.x);
			draggedItem.itemBounds.y = static_cast<int>(mousePos.y + draggedItem.offset.y);

		}
		else
		{
			// Drop the item
			m_InventoryScreen->handleItemDrop();
			m_InventoryScreen->checkCraftingOutput();
			draggedItem.isDragging = false;
			draggedItem.stack.clear();
		}
}

}

void UIManager::Render()
{
	m_InventoryScreen->Render(*m_Renderer2D);
	m_PlayerGameInv->Render(*m_Renderer2D);

	RenderDraggedItem();
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

uint8_t UIManager::GetItemIDInHotBarIndex() const
{
	int index = m_PlayerGameInv->getCurrentIndex();
	const ItemStack& stack = m_PlayerGameInv->playerHotbarInv->getSlot(index);
	return static_cast<uint8_t>(stack.itemID);
}

void UIManager::RenderDraggedItem()
{
	// Render dragged item on top if any
	DraggedItem& draggedItem = m_InventoryScreen->draggedItem;
	if (draggedItem.isDragging)
	{
		if (!draggedItem.stack.isEmpty())
		{
			AtlasPos texPos = g_ItemTypes[draggedItem.stack.itemID].inventoryTextureIndex;
			static int atlasHeight = 512;
			static int cellSize = 16;
			int y = texPos.y;
			int flippedY = (atlasHeight / cellSize - 1) - y;
			m_Renderer2D->drawSprite(Sprite{ texPos.x * 16, flippedY * 16, 16, 16 },
				static_cast<float>(draggedItem.itemBounds.x),
				static_cast<float>(draggedItem.itemBounds.y),
				static_cast<float>(draggedItem.itemBounds.w),
				static_cast<float>(draggedItem.itemBounds.h),
				0xFFFFFFFF, true);
		}
	}
}
