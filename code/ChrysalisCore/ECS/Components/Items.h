#pragma once

#include <ECS/Components/Components.h>


namespace Chrysalis::ECS
{
struct ItemClass
{
	ItemClass() = default;
	virtual ~ItemClass() = default;


	void Serialize(Serialization::IArchive& ar)
	{
		ar(maxStackSize, "maxStackSize", "maxStackSize");
		ar(animationTag, "animationTag", "animationTag");
		ar(isUniqueInventory, "isUniqueInventory", "isUniqueInventory");
		ar(isUniqueEquipment, "isUniqueEquipment", "isUniqueEquipment");
		ar(isDroppable, "isDroppable", "isDroppable");
		ar(isAutoDroppable, "isAutoDroppable", "isAutoDroppable");
		ar(isPickable, "isPickable", "isPickable");
		ar(isAutoPickable, "isAutoPickable", "isAutoPickable");
		ar(isUsable, "isUsable", "isUsable");
		ar(isTradable, "isTradable", "isTradable");
		ar(isConsumable, "isConsumable", "isConsumable");
	}


	/** How many items may be placed into a stack. */
	uint32 maxStackSize {1};

	/** A tag(s) to apply to mannequin when this item is in use. */
	string animationTag;

	/** true if this item is unique within an inventory. */
	bool isUniqueInventory {true};

	/** true if this item is unique when equipped. */
	bool isUniqueEquipment {true};

	/** Is this entity droppable? */
	bool isDroppable {true};

	/** Is this entity droppable automatically? */
	bool isAutoDroppable {false};

	/** true if this object is able to be picked up. */
	bool isPickable {true};

	/** true if this object is automatically able to be picked up. */
	bool isAutoPickable {true};

	/** true if this object is usable. */
	bool isUsable {true};

	/** Is this entity able to be bought / picked up / given / traded to a character? */
	bool isTradable {true};

	/** Is this entity consumable? */
	bool isConsumable {false};
};
}