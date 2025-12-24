#include "InventoryScreen.h"
#include "UITypes.h"

InventoryScreen::~InventoryScreen()
{
}

void InventoryScreen::onOpen()
{
	// Clear existing widgets
	m_Widgets.clear();
	
	// Temp ui scale will have setting parameter in future
	const int uiScale = 1;

	// Define grid parameters
	const int cols = 9;
	const int rows = 4;

	const int baseSlotSize = 48;
	const int slotSize = int(baseSlotSize * uiScale);
	const int gridWidth = cols * slotSize;
	const int gridHeight = rows * slotSize;
	const int startX = (m_ScreenW - gridWidth) / 2;
	const int startY = (m_ScreenH - gridHeight) / 2;
	// Create slots
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			int index = row * cols + col;
			Rect slotRect{ startX + col * slotSize, startY + row * slotSize, slotSize, slotSize };
			auto slot = std::make_unique<UISlot>(index, slotRect, playerInventory);
			m_Widgets.push_back(std::move(slot));
		}
	}

	// Set flag
	m_Open = true;

	// Labels if I want to add them later here
}

void InventoryScreen::onClose()
{
	// Clear widgets when closing the screen
	m_Widgets.clear();

	// Set flag
	m_Open = false;
}