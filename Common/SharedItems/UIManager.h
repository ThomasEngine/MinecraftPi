#pragma once  
#include <memory>
#include "InventoryScreen.h"
#include "PlayerGameInv.h"

class Renderer2D;  
class Container;  
class Input;  
class Game;  
class UIManager  
{  
public:  
	UIManager() {}  
	~UIManager() { Shutdown(); }  

	void Initialize(Container& playerInv, Game& game, Renderer2D& ren);
	void Shutdown();  

	void Update(float deltaTime, const Input& input);  
	void Render();  

	void SetWindowSize(int width, int height);
	void OpenInventory();
	void CloseInventory();
private:  
	Renderer2D* m_Renderer2D;  

	// UI's  
	std::unique_ptr<InventoryScreen> m_InventoryScreen;
	std::unique_ptr<PlayerGameInv> m_PlayerGameInv;

	Game* m_Owner;  

	// Internal data for UI management  
};
