#include "CraftingRegistry.h"

CraftingRecipe g_CraftingRecipes[ItemTypeId::I_AMOUNT];

void InitializeCraftingRecipes()
{
	// Wooden planks
	// Shapeless recipe: 1 Oak Log to 4 Oak Planks
	g_CraftingRecipes[ItemTypeId::I_OAK_PLANK_BLOCK] = {
		ItemTypeId::I_OAK_PLANK_BLOCK, 4,
		{ ItemTypeId::I_OAK_LOG_BLOCK, ItemTypeId::I_NONE, ItemTypeId::I_NONE,
		  ItemTypeId::I_NONE,        ItemTypeId::I_NONE, ItemTypeId::I_NONE,
		  ItemTypeId::I_NONE,        ItemTypeId::I_NONE, ItemTypeId::I_NONE },
		CRAFTING_RECIPE_SHAPELESS,
		CRAFTING_SIZE_4
	};
}
