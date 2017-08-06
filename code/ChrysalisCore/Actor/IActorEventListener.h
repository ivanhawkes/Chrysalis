/**
Declares the IActorEventListener interface. This interface handles events that are raised from the
movement code and provides the means for that code to notify the actor of useful events.

NOTE: These are the events which were used in GameSDK - we will remove ones not needed by Chrysalis
over time, and add any that are specific to characters or classes derived from actor as needed
e.g. ICharacterEventListener : public IActorEventListener
*/
#pragma once

#include <CryAISystem/IAgent.h>


struct IEntityClass;


namespace Chrysalis
{
struct IActor;


// TODO: CRITICAL: HACK: BROKEN: !! This functionality needs reworking now. It can't use IActor anymore as we no longer
// support that, but it probably doesn't need any parameters of IActor type, since it should most likely be implemented
// by a new component that is the actor - thus not needing actor passed in. Need to look into this in detail.
// 
// A good alternative will be to define our own IActor class as the base class for CCharacterComponent / CPetComponent / etc, but will
// need namespaces sorted for that. 

struct IActorEventListener
{
	enum ESpecialMove
	{
		eSM_Jump = 0,
		eSM_SpeedSprint,
	};

	IActorEventListener() {}
	virtual ~IActorEventListener() {}


	/**
	Executes the special move action. Special moves are defined in an enum.

	\param [in,out]	pActor If non-null, the actor.
	\param	move		   The move.
	**/

	// #TODO: Since there are just two special moves, consider making them full events instead.
	virtual void OnSpecialMove(IActor* pActor, IActorEventListener::ESpecialMove move) = 0;


	/**
	The actor has died.

	\param [in,out]	pActor If non-null, the actor.
	\param	bIsGod		   true if this instance is god.
	**/
	virtual void OnDeath(IActor* pActor, bool bIsGod) = 0;


	/**
	The actor has been revived.

	\param [in,out]	pActor If non-null, the actor.
	\param	bIsGod		   true if this instance is god.
	**/
	virtual void OnRevive(IActor* pActor, bool bIsGod) = 0;


	/**
	The actor has entered a vehicle.

	\param [in,out]	pActor	    If non-null, the actor.
	\param	strVehicleClassName Name of the vehicle class.
	\param	strSeatName		    Name of the seat.
	\param	bThirdPerson	    true if we are in third person.
	**/
	virtual void OnEnterVehicle(IActor* pActor, const char* strVehicleClassName, const char* strSeatName, bool bThirdPerson) = 0;


	/**
	The actor has exited a vehicle.

	\param [in,out]	pActor If non-null, the actor.
	**/
	virtual void OnExitVehicle(IActor* pActor) = 0;


	/**
	The actor's health has changed.

	\param [in,out]	pActor If non-null, the actor.
	\param	newHealth	   The new health.
	**/
	virtual void OnHealthChanged(IActor* pActor, float newHealth) = 0;


	/**
	The actor has picked up an item.

	\param [in,out]	pActor If non-null, the actor.
	\param	itemId		   Identifier for the item.
	**/
	virtual void OnItemPickedUp(IActor* pActor, EntityId itemId) = 0;


	/**
	The actor has used an item.

	\param [in,out]	pActor If non-null, the actor.
	\param	itemId		   Identifier for the item.
	**/
	virtual void OnItemUsed(IActor* pActor, EntityId itemId) = 0;


	/**
	The actor has dropped an item.

	\param [in,out]	pActor If non-null, the actor.
	\param	itemId		   Identifier for the item.
	**/
	virtual void OnItemDropped(IActor* pActor, EntityId itemId) = 0;


	/**
	The actor has changed stance.

	\param [in,out]	pActor If non-null, the actor.
	\param	stance		   The stance.
	**/
	virtual void OnStanceChanged(IActor* pActor, EStance stance) = 0;


	/**
	The actor has toggled between third person and first person view modes.

	\param [in,out]	pActor If non-null, the actor.
	\param	bThirdPerson   true to third person.
	**/
	virtual void OnToggleThirdPerson() {};


	/**
	The actor's sprint stamina has changed.

	\param [in,out]	pActor If non-null, the actor.
	\param	newStamina	   The new stamina.
	**/
	virtual void OnSprintStaminaChanged(IActor* pActor, float newStamina) = 0;
};
}