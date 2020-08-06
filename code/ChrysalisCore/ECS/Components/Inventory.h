#pragma once

#include <ECS/Components/Components.h>


namespace Chrysalis::ECS
{
struct Inventory
{
	/** Maximum entries for this inventory. */
	uint32 maxEntries {40};
};


struct InventoryEntry
{
	/** A reference to the item class this entry represents. */
	string itemClassName;

	/** The quantity. */
	uint32 quantity {1};
};


struct InventoryEntryList
{
	/** A container for all the entries. */
	std::vector<InventoryEntry> entries;
};
}