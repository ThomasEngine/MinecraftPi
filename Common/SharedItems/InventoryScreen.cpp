#include "InventoryScreen.h"
#include "Crafting.h"

InventoryScreen::InventoryScreen(Container* inv)
	: playerInventory(inv)
{
	craftingContainer = new Container();
	craftingContainer->setSize(5); // 2x2 crafting grid + 1 output
	craftingContainer->clear();
}

InventoryScreen::~InventoryScreen()
{
}

void InventoryScreen::handleItemDrop(int amount)
{
	for  (auto& widget : m_Widgets)
	{
		if (widget->hovered) 
		{
			UISlot* slot = dynamic_cast<UISlot*>(widget.get());
			if (slot && slot->container)
			{
				Container* container = slot->container;
				DraggedItem& draggedItem = slot->draggedItem;
				if (draggedItem.isDragging)
				{
					// Place item in this slot
					printf("Placing item in slot %d\n", slot->slotIndex);
					ItemStack& targetStack = container->getSlot(slot->slotIndex);
					if (targetStack.isEmpty())
					{
						printf("Target stack is empty\n");
						// if amount is -1 take all
						if (amount > 0 && amount < draggedItem.stack.quantity)
						{
							targetStack.itemID = draggedItem.stack.itemID;
							targetStack.quantity = amount;
							draggedItem.stack.quantity -= amount;
							return;
						}
						else if (amount == -1 || amount >= draggedItem.stack.quantity)
						{
							std::swap(targetStack, draggedItem.stack);
							return;
						}
					}
					else
					{
						// try to add to existing stack if same item
						if (targetStack.itemID == draggedItem.stack.itemID)
						{
							int spaceLeft = 64 - targetStack.quantity; // assuming max stack size is 64
							if (spaceLeft > 0)
							{
								if (amount > 0 && amount < draggedItem.stack.quantity)
								{
									int toTransfer = std::min(spaceLeft, amount);
									targetStack.quantity += toTransfer;
									draggedItem.stack.quantity -= toTransfer;
									return;
								}
							}
						}
					}
				}
			}
		}
	}
}

void InventoryScreen::checkCraftingOutput()
{
	// Check crafting container for valid recipe
	ReturnCrafting result = Crafting::canCraft(craftingContainer, craftingMinSize::CRAFTING_SIZE_4);
	printf("Crafting check result: Item %s, Quantity %d\n", g_ItemTypes[result.itemID].name.c_str(), result.quantity);
	// Update output slot (index 4)
	if (result.itemID != ItemTypeId::I_NONE)
	{
		ItemStack& outputSlot = craftingContainer->getSlot(4);
		outputSlot.itemID = result.itemID;
		outputSlot.quantity = result.quantity;
	}
	else
	{
		// Clear output slot
		ItemStack& outputSlot = craftingContainer->getSlot(4);
		outputSlot.clear();
	}
}

void InventoryScreen::onOpen()
{
	// Clear existing widgets
	m_Widgets.clear();
	
	// Temp ui scale will have setting parameter in future
	const int uiScale = 1;

	// Define grid parameters
	int cols = 9;
	int rows = 4;
	const int inBetweenSlotSpace = 3;
	const int baseSlotSize = 48;
	const int slotSize = int(baseSlotSize * uiScale);

	// Calculate grid size including spacing
	const int gridWidth = cols * slotSize + (cols - 1) * inBetweenSlotSpace;
	const int gridHeight = rows * slotSize + (rows - 1) * inBetweenSlotSpace;
	const int backgroundHeight = gridHeight + 24 * 7; // extra for top and bottom and padding
	const int backgroundWidth = gridWidth + 40;
	const int startXBackground = (m_ScreenW - backgroundWidth) / 2;
	const int startYBackground = (m_ScreenH - backgroundHeight) / 2;
	const int startX = (m_ScreenW - gridWidth) / 2;
	const int startY = startYBackground + slotSize * 3.5; // start a bit lower for padding

	// First add background
	// Background is twice the height and a bit wider than the grid
	Rect backgroundRect{ startXBackground, startYBackground, backgroundWidth, baseSlotSize };
	auto background = std::make_unique<UISlot>(-1, backgroundRect, nullptr, SlotTypes::BackgroundTop, draggedItem);
	m_Widgets.push_back(std::move(background));

	// Middle part
	int startYBackgroundMiddle = startYBackground + 24;
	Rect backgroundMiddleRect{ startXBackground, startYBackgroundMiddle, backgroundWidth, backgroundHeight - 24 * 2 };
	auto backgroundMiddle = std::make_unique<UISlot>(-1, backgroundMiddleRect, nullptr, SlotTypes::BackgroundMiddle, draggedItem);
	m_Widgets.push_back(std::move(backgroundMiddle));

	// Bottom part
	int startYBackgroundBottom = startYBackground + backgroundHeight - 24;
	Rect backgroundBottomRect{ startXBackground, startYBackgroundBottom, backgroundWidth, baseSlotSize };
	auto backgroundBottom = std::make_unique<UISlot>(-1, backgroundBottomRect, nullptr, SlotTypes::BackgroundBottom, draggedItem);
	m_Widgets.push_back(std::move(backgroundBottom));

	// Create slots
	// Normal inventory slots
	int slotX, slotY;
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			int index = row * cols + col;
			slotX = startX + col * (slotSize + inBetweenSlotSpace);
			slotY = startY + row * (slotSize + inBetweenSlotSpace);
			slotY = (row == (rows - 1)) ? slotY + slotSize * 0.20 : slotY;
			Rect slotRect{ slotX, slotY, slotSize, slotSize };
			auto slot = std::make_unique<UISlot>(index, slotRect, playerInventory, SlotTypes::Inventory, draggedItem);
			m_Widgets.push_back(std::move(slot));
		}
	}

	// Crafting slot 2x2 input with arrow to 1 output input and output are normal slots
	// Place crafting grid in the top right, with some margin
	// Place crafting grid at a fixed offset from the top-right of the background
	const int craftingMarginX = 144;
	const int craftingMarginY = 34;
	int craftingStartX = startXBackground + backgroundWidth - (2 * slotSize + inBetweenSlotSpace) - craftingMarginX;
	int craftingStartY = startYBackground + craftingMarginY;

	// Input slots
	rows = 2;
	cols = 2;
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			int index = row * cols + col; // after inventory slots
			slotX = craftingStartX + col * (slotSize + inBetweenSlotSpace);
			slotY = craftingStartY + row * (slotSize + inBetweenSlotSpace);
			Rect slotRect{ slotX, slotY, slotSize, slotSize };
			auto slot = std::make_unique<UISlot>(index, slotRect, craftingContainer, SlotTypes::Inventory, draggedItem);
			m_Widgets.push_back(std::move(slot));
		}
	}

	// Output slot
	int outputIndex = 4; // output slot index in crafting container
	int outputSlotX = craftingStartX + 2 * (slotSize + inBetweenSlotSpace) + slotSize + slotSize * 0.3f;
	int outputSlotY = craftingStartY + slotSize / 2; // vertically centered
	Rect outputSlotRect{ outputSlotX, outputSlotY, slotSize, slotSize };
	auto outputSlot = std::make_unique<UISlot>(outputIndex, outputSlotRect, craftingContainer, SlotTypes::Inventory, draggedItem);
	m_Widgets.push_back(std::move(outputSlot));

	// Arrow in between
	int arrowX = craftingStartX + 2 * (slotSize + inBetweenSlotSpace) + slotSize * 0.15;
	int arrowY = craftingStartY + slotSize - slotSize / 2; // center arrow vertically
	Rect arrowRect{ arrowX, arrowY, slotSize, slotSize };
	auto arrowSlot = std::make_unique<UISlot>(-1, arrowRect, nullptr, SlotTypes::CraftingArrow, draggedItem);
	m_Widgets.push_back(std::move(arrowSlot));

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