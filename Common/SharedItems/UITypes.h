#pragma once
#include "BaseUI.h"
#include <string>
#include "ItemRegistry.h"

class UIButton : public UIWidget {
public:
	Rect bounds;
	std::string label;
	bool isHovered = false;
	bool isPressed = false;

	void Update(const Input* input, float deltaTime) override;
	void Render(Renderer2D& ren) const override;
	void OnClick();
};

// class UILabel : public UIWidget {


// Slots
struct ItemStack {
	ItemTypeId itemID;
	int quantity;
	bool isEmpty() const { return quantity <= 0; }
};

class Container {
public:
	Container() {
		slots.resize(maxSlots);
	}
	void AddItem(ItemTypeId itemID, int quantity, int slotIndex = -1);
	
	size_t getSlotCount() const { return slots.size(); }
	ItemStack& getSlot(size_t index) { return slots[index]; }
private:
	std::vector<ItemStack> slots;
	int maxStackSize = 64;
	int maxSlots = 36;
};

enum class SlotTypes {
	Inventory,
	HotBar,
	HotBarSelect,
	BackgroundTop,
	BackgroundMiddle,
	BackgroundBottom,
	CraftingArrow,
	HelmetHolder,
	ChestplateHolder,
	LeggingsHolder,
	BootsHolder,
	Count
};


enum class MouseButtons;
class UISlot : public UIWidget {
public:
	UISlot(int index, const Rect& rect, Container* cont, SlotTypes type = SlotTypes::Inventory)
		: slotIndex(index), bounds(rect), container(cont), slotType(type) {
	}
	Rect bounds;
	int slotIndex;
	Container* container = nullptr;
	SlotTypes slotType;

	bool hovered = false;
	void Update(const Input* input, float deltaTime) override;
	void Render(Renderer2D& ren) const override;

private:
	void clicked(MouseButtons button, const Input& input);
};