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
	const int backgroundHeight = gridHeight + 24 * 7; // extra for top and bottom and padding
	const int backgroundWidth = gridWidth + 40;
	const int startXBackground = (m_ScreenW - backgroundWidth) / 2;
	const int startYBackground = (m_ScreenH - backgroundHeight) / 2;
	const int startX = (m_ScreenW - gridWidth) / 2;
	const int startY = startYBackground + 24 * 5.75; // start a bit lower for padding

	// First add background
	// Background is twice the height and a bit wider than the grid
	Rect backgroundRect{ startXBackground, startYBackground, backgroundWidth, 24};
	auto background = std::make_unique<UISlot>(-1, backgroundRect, nullptr, SlotTypes::BackgroundTop);
	m_Widgets.push_back(std::move(background));

	// Middle part
	int startYBackgroundMiddle = startYBackground + 24;
	Rect backgroundMiddleRect{ startXBackground, startYBackgroundMiddle, backgroundWidth, backgroundHeight - 24 * 2 };
	auto backgroundMiddle = std::make_unique<UISlot>(-1, backgroundMiddleRect, nullptr, SlotTypes::BackgroundMiddle);
	m_Widgets.push_back(std::move(backgroundMiddle));

	// Bottom part
	int startYBackgroundBottom = startYBackground + backgroundHeight - 24;
	Rect backgroundBottomRect{ startXBackground, startYBackgroundBottom, backgroundWidth, 24 };
	auto backgroundBottom = std::make_unique<UISlot>(-1, backgroundBottomRect, nullptr, SlotTypes::BackgroundBottom);
	m_Widgets.push_back(std::move(backgroundBottom));

	// Create slots
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			int index = row * cols + col;
			int modY = row == (rows - 1) ? startY + row * slotSize + slotSize * 0.25 : startY + row * slotSize;
			Rect slotRect{ startX + col * slotSize, modY, slotSize, slotSize };
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