#include "Crafting.h"
#include "InventoryScreen.h"
#include <vector>

namespace {
// Helper functions can go here

	CraftingRecipe findMatchingShapedRecipe(Container* craftingInv, craftingMinSize size)
	{
		int craftingSize = (size == craftingMinSize::CRAFTING_SIZE_4) ? 2 : 3;
		std::vector<ItemTypeId> inputItems(craftingSize * craftingSize);

		for (const auto& recipe : g_CraftingRecipes) {
			if (recipe.recipeType != CRAFTING_RECIPE_SHAPED || recipe.minSize != size) {
				continue;
			}
			bool match = true;
			for (int row = 0; row < craftingSize; ++row) {
				for (int col = 0; col < craftingSize; ++col) {
					int invIndex = row * craftingSize + col;
					int recipeIndex = row * 3 + col; // recipe is always 3x3
					ItemTypeId invItem = craftingInv->getSlot(invIndex).itemID;
					ItemTypeId recipeItem = recipe.inputItems[recipeIndex];
					if (invItem != recipeItem) {
						match = false;
						break;
					}
				}
				if (!match) break;
			}
			if (match) {
				CraftingRecipe returnRecipe = recipe; // copy to return
				return returnRecipe;
			}
		}
		return { ItemTypeId::I_NONE, 0, {}, CRAFTING_RECIPE_SHAPED, size };
	}

	CraftingRecipe findMatchingShapelessRecipe(Container* craftingInv, craftingMinSize size)
	{
		// Shapeless recipe
		ItemTypeId inputItems[9];
		ItemTypeId recipeItems[9];

		for (size_t i = 0; i < 9; ++i) {
			ItemStack& slot = craftingInv->getSlot(i);
			inputItems[i] = slot.itemID;
			if (slot.isEmpty()) {
				inputItems[i] = ItemTypeId::I_NONE;
			}
		}

		int recipes = 0;
		std::sort(std::begin(inputItems), std::end(inputItems));
		for (const auto& recipe : g_CraftingRecipes) {
			for (size_t i = 0; i < 9; ++i) {
				printf("Recipe Item %zu: %d\n", i, recipe.inputItems[i]);
				if (recipe.inputItems[i] != ItemTypeId::I_NONE) {
					printf("Recipe Item Name: %s\n", g_ItemTypes[recipe.inputItems[i]].name.c_str());
				}
				recipeItems[i] = recipe.inputItems[i];
			}
			recipes++;
			std::sort(std::begin(inputItems), std::end(inputItems));
			std::sort(std::begin(recipeItems), std::end(recipeItems));
			if (std::equal(std::begin(recipeItems), std::end(recipeItems), std::begin(inputItems))) {
				CraftingRecipe returnRecipe = recipe;
				return returnRecipe;
			}
		}
		printf("Total shapeless recipes checked: %d\n", recipes);
		return { ItemTypeId::I_NONE, 0, {}, CRAFTING_RECIPE_SHAPELESS, size };
	}
}

ReturnCrafting Crafting::canCraft(Container* craftingInv, craftingMinSize size)
{
	printf("Start check crafting recipe...\n");
	// First check shaped recipes
	printf("Checking shaped recipes...\n");
	CraftingRecipe shapedRecipe = findMatchingShapedRecipe(craftingInv, size);
	if (shapedRecipe.outputItem != ItemTypeId::I_NONE) {
		printf("Shaped recipe matched: Item %s, Quantity %d\n", g_ItemTypes[shapedRecipe.outputItem].name.c_str(), shapedRecipe.outputQuantity);
		return { shapedRecipe.outputItem, shapedRecipe.outputQuantity };
	}
	// No shaped recipe matched, check shapeless
	printf("No shaped recipe matched, checking shapeless recipes...\n");
	CraftingRecipe shapelessRecipe = findMatchingShapelessRecipe(craftingInv, size);
	if (shapelessRecipe.outputItem != ItemTypeId::I_NONE) {
		printf("Shapeless recipe matched: Item %s, Quantity %d\n", g_ItemTypes[shapelessRecipe.outputItem].name.c_str(), shapelessRecipe.outputQuantity);
		return { shapelessRecipe.outputItem, shapelessRecipe.outputQuantity };
	}
	printf("No shapeless recipe matched.\n");
	// No matching recipe found
	return { ItemTypeId::I_NONE, 0 };
}
