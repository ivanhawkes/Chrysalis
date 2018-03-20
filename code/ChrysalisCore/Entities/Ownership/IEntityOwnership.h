/**
\file	D:\Chrysalis\Source\Chrysalis\Entity\Owner\EntityOwner.h

Declares an interface for handling ownership of entities.
*/
#pragma once

#include <CryEntitySystem/IEntity.h>


namespace Chrysalis
{
class IEntityOwnership
{
public:
	IEntityOwnership() = default;
	virtual ~IEntityOwnership() = default;


	/** All notions of ownership are reset to their default state. */
	virtual void Reset() {};


	/**
	Returns the entity which owns this particular entity.

	\return	The EntityId of it's owning entity or INVALID_ENTITYID if it isn't currently owned.
	*/
	virtual EntityId IsOwnedBy() { return INVALID_ENTITYID; };

	// TODO: Add in basic concepts of trade, picking up, locks, etc...add new classes that implement this as required.
};
}