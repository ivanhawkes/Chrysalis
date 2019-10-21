#pragma once

#include <CrySchematyc/Reflection/TypeDesc.h>


/**
Exposes the methods needed for management of items that can be added to an inventory.

**/

namespace Chrysalis
{

/** Allows for handling of customised items. */
class IItemCustomisation
{
public:

};


/** Item receipt's allow for the storage and transfer of items between interested parties. */
class IItemReceipt
{
public:
	static void ReflectType(Schematyc::CTypeDesc<IItemReceipt>& desc)
	{
		desc.SetGUID("{520CBE44-98C8-4885-9F63-918E001F560C}"_cry_guid);
		desc.SetLabel("Item Receipt Properties");
		desc.SetDescription("Item Receipt.");
	}

	virtual void Serialize(Serialization::IArchive& ar)
	{
		Serialization::SContext context(ar, this);
		ar(itemClass, "ItemClass", "Item Class");
		ar(quantity, "quantity", "quantity");
	}

	/** A reference to the item's class, which holds common information for this sort of item. */
	string itemClass;
	
	uint32 quantity;

	IItemCustomisation ItemCustomisation;
};



class IItem
{
public:
};
}