#include "UITypes.h"
#include "../Input.h"
#include "../IInput.h"
#include "Renderer2D.h"

namespace {
	glm::ivec2 GetSloteTypeAtlas(SlotTypes type)
	{
		switch (type)
		{
		case SlotTypes::Inventory:
			return { 0,0 };
			break;
		case SlotTypes::HotBar:
			return {1,0};
			break;
		case SlotTypes::HotBarSelect:
			return {2,0};
			break;
		case SlotTypes::BackgroundTop:
			return { 10,0 };
			break;
		case SlotTypes::BackgroundMiddle:
			return { 0,1 };
			break;
		case SlotTypes::BackgroundBottom:
			return { 3,0 };
			break;
		case SlotTypes::HelmetHolder:
			return {7,1};
			break;
			break;
		case SlotTypes::ChestplateHolder:
			return {8,1};
			break;
		case SlotTypes::LeggingsHolder:
			return { 9,1 };
			break;
		case SlotTypes::BootsHolder:
			return {10,1};
			break;
		case SlotTypes::CraftingArrow:
			return { 11,1 };
			break;
		default:
			return {0,0};
			break;
		}
	}

	static const float waitForSecondClick = 10.f;
	static float timer = 0.0f;

	bool isSlotBackground(SlotTypes type)
	{
		return type == SlotTypes::BackgroundTop || type == SlotTypes::BackgroundMiddle || type == SlotTypes::BackgroundBottom;
	}
}

void UIButton::Update(const Input* input, float deltaTime)
{
	const IMouse& mouse = input->GetMouse(); // Get mouse from input
	isHovered = bounds.contains(mouse.GetPosition()); // Check if mouse is over button
	if (isHovered && mouse.GetButtonDown(MouseButtons::LEFT)) isPressed = true; // Check if button is pressed
	if (isPressed && !mouse.GetButtonDown(MouseButtons::LEFT)) { // On mouse release
		if (isHovered) OnClick(); // Trigger click if still hovered
		isPressed = false;
	}
}

void UIButton::Render(Renderer2D& ren) const
{
	// Render button background
	uint32_t bgColor = isPressed ? 0xFFAAAAAA : (isHovered ? 0xFFCCCCCC : 0xFFFFFFFF);
	ren.drawQuad(float(bounds.x), float(bounds.y), float(bounds.w), float(bounds.h), 0.0f, 0.0f, 1.0f, 1.0f, bgColor);
	// Render button label (centered)
	// For simplicity, we won't implement text rendering here
	// In a real implementation, you would use a text rendering function	
}

void UIButton::OnClick()
{
	// Placeholder for button click action
}

void UISlot::Update(const Input* input, float deltaTime)
{
	const IMouse& mouse = input->GetMouse();
	hovered = bounds.contains(mouse.GetPosition());

	timer += deltaTime;

	if (!hovered || !container) return;

	if (hovered && mouse.GetButtonDown(MouseButtons::LEFT)) {
		clicked(MouseButtons::LEFT, *input, deltaTime);
	}
	else if (hovered && mouse.GetButtonDown(MouseButtons::RIGHT)) {
		clicked(MouseButtons::RIGHT, *input, deltaTime);
	}
	else if (hovered && mouse.GetButtonDown(MouseButtons::MIDDLE)) {
		clicked(MouseButtons::MIDDLE, *input, deltaTime);
	}
}

void UISlot::Render(Renderer2D& ren) const
{
	// Draw base
	bool isBackground = isSlotBackground(slotType);
	int width = isBackground ? 24 * 7 : 24;

	uint32_t slotColor = isBackground ? 0xFFFFFFFF : (hovered ? 0x33EBEBEB : 0xFFFFFFFF);

	glm::ivec2 atlasPos = GetSloteTypeAtlas(slotType);
	ren.drawSprite(Sprite{ atlasPos.x * 24, 232 - atlasPos.y * 24, width, 24}, float(bounds.x), float(bounds.y), float(bounds.w), float(bounds.h), slotColor);
	if (!container) return;

	const ItemStack& stack = container->getSlot(slotIndex);
	if (!stack.isEmpty()) {
		AtlasPos texPos = g_ItemTypes[stack.itemID].inventoryTextureIndex;
		// Make bounds slightly smaller to fit inside slot
		Rect itemBounds = { bounds.x + 4, bounds.y + 4, bounds.w - 8, bounds.h - 8 };
		static int atlasHeight = 512;
		static int cellSize = 16;
		int y = texPos.y;
		int flippedY = (atlasHeight / cellSize - 1) - y;

		ren.drawSprite(Sprite{ texPos.x * 16, flippedY * 16, 16, 16 }, float(itemBounds.x), float(itemBounds.y), float(itemBounds.w), float(itemBounds.h), 0xFFFFFFFF, true);
	}
}

void UISlot::clicked(MouseButtons button, const Input& input, float deltaTime)
{
	printf("Timer time: %.2f\n", timer);
	if (timer < waitForSecondClick) {
		return;
	}
	timer = 0.0f;

	printf("CLICK\n");

	if (!container) return;

	ItemStack& slotStack = container->getSlot(slotIndex);

	// If currently dragging, try to place
	if (draggedItem.isDragging) {
		// Only place if slot is empty
		if (slotStack.isEmpty()) {
			slotStack = draggedItem.stack;
			draggedItem.stack.clear();
			draggedItem.isDragging = false;
			draggedItem.active = false;
			printf("Place\n");
		}
		else
		{
			// Swap items keep dragging but with new item
			std::swap(slotStack, draggedItem.stack);
			printf("Swap\n");
		}
		draggedItem.onItemDrop(); // Call drop callback
	}
	// If not dragging, try to pick up
	else if (!slotStack.isEmpty()) {
		draggedItem.stack = slotStack;
		draggedItem.itemBounds = Rect{ bounds.x + 4, bounds.y + 4, bounds.w - 8, bounds.h - 8 };
		slotStack.clear();
		draggedItem.isDragging = true;
		draggedItem.active = true;
		printf("Take\n");
	}
}


void Container::AddItem(ItemTypeId itemID, int quantity, int slotIndex)
{
	for (auto& slot : slots) {
		if (slot.itemID == itemID && slot.quantity + quantity <= maxStackSize) {
			slot.quantity += quantity;
			return;
		}
	}

	if (slotIndex >= 0 && slotIndex < static_cast<int>(slots.size())) {
		slots[slotIndex].itemID = itemID;
		slots[slotIndex].quantity += quantity;
	}
	else {
		// Find first empty slot
		for (auto& slot : slots) {
			if (slot.isEmpty()) {
				slot.itemID = itemID;
				slot.quantity = quantity;
				break;
			}
		}
	}
}
