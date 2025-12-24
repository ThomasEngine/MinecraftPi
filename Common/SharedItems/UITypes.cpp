#include "UITypes.h"
#include "Input.h"
#include "IInput.h"
#include "Renderer2D.h"

namespace {
	int GetSloteTypeAtlas(SlotTypes type)
	{
		switch (type)
		{
		case SlotTypes::Inventory:
			return 0;
			break;
		case SlotTypes::HotBar:
			return 24;
			break;
		case SlotTypes::HotBarSelect:
			return 48;
			break;
		default:
			return 0;
			break;
		}
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
	if (!hovered || !container) return;

	if (hovered && mouse.GetButtonDown(MouseButtons::LEFT)) {
		clicked(MouseButtons::LEFT, *input);
	}
	else if (hovered && mouse.GetButtonDown(MouseButtons::RIGHT)) {
		clicked(MouseButtons::RIGHT, *input);
	}
	else if (hovered && mouse.GetButtonDown(MouseButtons::MIDDLE)) {
		clicked(MouseButtons::MIDDLE, *input);
	}
}

void UISlot::Render(Renderer2D& ren) const
{
	// Draw base
	ren.drawSprite(Sprite{ GetSloteTypeAtlas(slotType), 0, 24, 24}, float(bounds.x), float(bounds.y), float(bounds.w), float(bounds.h), 0xFFFFFFFF);
	if (!container) return;

	const ItemStack& stack = container->getSlot(slotIndex);
	if (!stack.isEmpty()) {
		printf("Drawing container\n");
		// Render item sprite (placeholder)
		ren.drawSprite(Sprite{ 16, 0, 16, 16 }, float(bounds.x), float(bounds.y), float(bounds.w), float(bounds.h), 0xFFFFFFFF);
		// Render quantity (placeholder)
	}
}

void UISlot::clicked(MouseButtons button, const Input& input)
{
	// Placeholder for slot click action
}
