#pragma once
#include "CraftingRegistry.h"

struct ReturnCrafting {
	ItemTypeId itemID;
	int quantity;
};

class Container;
class Crafting
{
public:
	static ReturnCrafting canCraft(Container* craftingInv, craftingMinSize craftingSize);
};

