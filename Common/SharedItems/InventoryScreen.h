#pragma once
#include "BaseUI.h"

class Container;
class InventoryScreen : public UIScreen
{
public:
	Container* playerInventory = nullptr;
	Container* craftingContainer = nullptr;
	
	InventoryScreen(Container* inv);
	~InventoryScreen() override;

	void handleItemDrop(int amount = -1);
	
	void onOpen() override;
	void onClose() override;
	inline bool isOpen() const { return m_Open; }

private:
	bool m_Open = false;
};

