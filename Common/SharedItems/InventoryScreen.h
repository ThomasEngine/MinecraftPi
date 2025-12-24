#pragma once
#include "BaseUI.h"

class Container;
class InventoryScreen : public UIScreen
{
public:
	Container* playerInventory = nullptr;
	
	InventoryScreen(Container* inv) : playerInventory(inv) {}
	~InventoryScreen() override;
	void onOpen() override;
	void onClose() override;
	inline bool isOpen() const { return m_Open; }

private:
	bool m_Open = false;
};

