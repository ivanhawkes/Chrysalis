#pragma once

#include <ECS/Components/Components.h>


namespace Chrysalis::ECS
{
struct Inventory final
{
	/** Maximum entries for this inventory. */
	uint32 maxEntries {40};
};


struct InventoryEntry final
{
	/** A reference to the item class this entry represents. */
	string itemClassName;

	/** The quantity. */
	uint32 quantity {1};
};


struct InventoryEntryList final
{
	/** A container for all the entries. */
	std::vector<InventoryEntry> entries;
};
}