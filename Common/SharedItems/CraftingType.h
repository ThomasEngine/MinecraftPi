#pragma once
#include "ItemType.h"


enum CraftingRecipeType {
	CRAFTING_RECIPE_SHAPED = 0,
	CRAFTING_RECIPE_SHAPELESS
};

enum craftingMinSize { // crafting grid size
	CRAFTING_SIZE_4 = 4,
	CRAFTING_SIZE_9 = 9
};

struct CraftingRecipe {
	ItemTypeId outputItem;
	int outputQuantity;
	ItemTypeId inputItems[9]; // 3x3 grid
	CraftingRecipeType recipeType;
	craftingMinSize minSize;
};
