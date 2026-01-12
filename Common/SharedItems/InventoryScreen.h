#pragma once
#include "UITypes.h"

class InventoryScreen : public UIScreen
{
public:
	Container* playerInventory = nullptr;
	Container* craftingContainer = nullptr;

	DraggedItem draggedItem;
	
	InventoryScreen(Container* inv);
	~InventoryScreen() override;

	void Update(const Input* input, float deltaTime) override;

	void handleItemDrop(int amount = -1);
	void checkCraftingOutput();
	
	void onOpen() override;
	void onClose() override;
	inline bool isOpen() const { return m_Open; }

private:
	bool m_Open = false;
};

