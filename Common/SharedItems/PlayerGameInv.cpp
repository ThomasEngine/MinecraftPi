#include "PlayerGameInv.h"

PlayerGameInv::~PlayerGameInv()
{
}

void PlayerGameInv::onOpen()
{
	// Clear existing widgets
	m_Widgets.clear();

	// Temp ui scale will have setting parameter in future
	const int uiScale = 1;

	// Define grid parameters
	const int cols = 9;

	const int baseSlotSize = 48;
	const int slotSize = int(baseSlotSize * uiScale);
	const int gridWidth = cols * slotSize;
	const int gridHeight = slotSize;
	const int startX = (m_ScreenW - gridWidth) / 2;
	// have it at the buttom of the screen
	const int startY = m_ScreenH - gridHeight - 10; // 10 pixels from bottom
	// Create slots
	for (int col = 0; col < cols; ++col) {
		int index = col;
		Rect slotRect{ startX + col * slotSize, startY, slotSize, slotSize };
		auto slot = std::make_unique<UISlot>(index, slotRect, playerHotbarInv, SlotTypes::HotBar);
		m_Widgets.push_back(std::move(slot));
	}
	// Add another UISLot on top of current selected index to highlight it
	// Using SlotTypes::HotBarSelect for different texture
	// Calculate highlight position
	// Should be a bit bigger than normal slot
	int index = m_CurrentIndex;
	Rect highlightRect{ startX - 2 + index * slotSize, startY - 2, slotSize + 4, slotSize + 4 };
	auto highlightSlot = std::make_unique<UISlot>(index, highlightRect, playerHotbarInv, SlotTypes::HotBarSelect);
	m_Widgets.push_back(std::move(highlightSlot));

	// Set flag
	m_Open = true;
}

void PlayerGameInv::onClose()
{
	// Clear widgets
	m_Widgets.clear();

	// Set flag
	m_Open = false;
}

void PlayerGameInv::setCurrentIndex(int index)
{
	if (!m_Open) return;
	if (index < 9) m_CurrentIndex = index;
	onOpen();
}

void PlayerGameInv::upIndex()
{
	if (!m_Open) return;
	m_CurrentIndex > 8 ? m_CurrentIndex = 0 : m_CurrentIndex++;
	onOpen();
}

void PlayerGameInv::downIndex()
{
	if (!m_Open) return;
	m_CurrentIndex < 0 ? m_CurrentIndex = 8 : m_CurrentIndex--;
	onOpen();
}
