#pragma once

#include <entt/entt.hpp>


namespace Chrysalis
{
/** Implement this interface to allow an entity to be able to participant in the spell casting system. */

struct ISpellParticipant
{
	/**
	Gets the ECS entity identifier for this participant.

	\return entt::null if it fails, else the entity identifier.
	**/
	virtual const entt::entity GetECSEntity() const = 0;
};


/** React to events in the lifetime of a spell cast. */

struct ISpellcasting
{
	/**
	Called at the start of a spell cast. Generally called on a downward keypress.
	**/

	virtual void OnSpellStart() = 0;

	/**
	Called each game frame a spell is stil casting. This will be called multiple times, as long as the player is
	holding down the key / button.
	**/

	virtual void OnSpellTick() = 0;

	/**
	Called when a spellcast is interupted by something. Spell casting can continue, but with some penalty applied.
	**/
	virtual void OnSpellInterupt() = 0;

	/**
	Called when an spellcast fails or fizzles e.g. target out of range during channelling.
	**/

	virtual void OnSpellFail() = 0;

	/**
	Called when an interaction is completed normally. Generally called on an upward keypress.
	**/

	virtual void OnSpellComplete() = 0;
};


/** Interface to allow queueing a spell for execution. */
struct ISpellcastManager
{
	/** Queue a spell up, ready for casting. */
	virtual bool QueueSpellCast(std::shared_ptr<ISpellcasting> spellcasting) = 0;
};
}