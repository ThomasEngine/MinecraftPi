#pragma once  
#include <cmath>  
#include <algorithm> 
#include <glm/glm.hpp>
#include <vector>
#include <memory>

// Structure to hold GUI scale information
struct GuiScale  
{  
   int scale;  
   int width;  
   int height;  
};  

// Function to get GUI scale based on window and virtual dimensions virtual dimensions are the minimum required dimensions for the GUI to display correctly
GuiScale computeGuiScale(int windowW, int windowH, int minVirtualW, int minVirtualH);

// Simple rectangle structure for UI elements
struct Rect {
	int x, y, w, h;
	template<typename T> 
	bool contains(glm::ivec2 p) {
		return p.x >= x && p.x < x + w && p.y >= y && p.y < y + h;
	}
	bool contains(glm::vec2 p) {
		return p.x >= x && p.x < x + w && p.y >= y && p.y < y + h;
	}
};


class Input;
class Renderer2D;
class UIWidget { // Base class for UI widgets
public:
	virtual ~UIWidget() = default;
	virtual void Update(const Input* input, float deltaTime) {}
	virtual void Render(Renderer2D& ren) const = 0;

	bool hovered = false;
};

class UIScreen { // Base class for UI screens that contains widgets
public:
	virtual ~UIScreen() = default;

	virtual void onOpen() {} // Called when the screen is opened
	virtual void onClose() {} // Called when the screen is closed

	// Update and render all widgets in the screen
	virtual void Update(const Input* input, float deltaTime) {
		for (auto& w : m_Widgets) {
			w->Update(input, deltaTime);
		}
	}
	virtual void Render(Renderer2D& ren) const {
		for (const auto& w : m_Widgets) {
			w->Render(ren);
		}
	}

	void setScreenWidthAndHeight(int screenW, int screenH) { m_ScreenW = screenW; m_ScreenH = screenH; }
protected: // protected and not private to allow derived classes to add widgets
	std::vector<std::unique_ptr<UIWidget>> m_Widgets;
	int m_ScreenW, m_ScreenH;
};
 