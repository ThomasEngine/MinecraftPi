#pragma once
#include "UITypes.h"

class PlayerGameInv : public UIScreen
{
public:
	Container* playerHotbarInv = nullptr;

	PlayerGameInv(Container* inv) : playerHotbarInv(inv) {}
	~PlayerGameInv() override;
	void onOpen() override;
	void onClose() override;

	void setCurrentIndex(int index);
	void upIndex();
	void downIndex();

	inline int getCurrentIndex() const { return m_CurrentIndex; }
	inline bool isOpen() const { return m_Open; }
private:
	
	int m_CurrentIndex = 0;
	bool m_Open = true; // open on start off game
};

