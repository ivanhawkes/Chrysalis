#pragma once

#include <CrySchematyc/Reflection/TypeDesc.h>
#include <entt/entt.hpp>


struct IActionController;
struct SActorMannequinParams;
class IAction;
struct ICharacterInstance;


namespace Chrysalis
{
class CPlayerComponent;
struct IInteraction;


/**
A very simple base class for all actors. This replaces the functionality needed by old crytek
code, as well as adding new functionality we require.
**/
struct IActor
{
	/**
	Is this actor being controlled by the local player?

	\returns A const bool.
	**/
	virtual const bool IsPlayer() const = 0;


	/**
	Is this actor being controlled by the local player?

	\return A const bool.
	**/
	virtual const bool IsClient() const = 0;


	/**
	Get's the player controlling this actor, if there is one. This may return nullptr.

	\return Null if it fails, else the player.
	**/
	virtual CPlayerComponent* GetPlayer() const = 0;


	/**
	Gets the character instance for this actor.

	\return Null if it fails, else the character.
	**/
	virtual ICharacterInstance* GetCharacter() const = 0;


	/**
	Gets this instance's local-space eye position (for a human, this is typically Vec3 (0, 0, 1.76f)).

	The code will first attempt to return a "Camera" helper if there is one. If only one eye is available (left_eye,
	right_eye) then that is used as the local position. In the case of two eyes, the position is the average of the two
	eyes.

	Position is calculated each time using the attachment manager, so it will be better to cache the results if you need
	to call this a few times in an update.

	\return This instance's local-space eye position.
	**/
	virtual const Vec3 GetLocalEyePos() const { return {0.0f, 0.0f, 1.82f}; };


	/**
	Gets local left hand position.

	\return The local left hand position.
	**/
	virtual Vec3 GetLocalLeftHandPos() const { return Vec3(ZERO); };


	/**
	Gets local right hand position.

	\return The local right hand position.
	**/
	virtual Vec3 GetLocalRightHandPos() const { return Vec3(ZERO); };

	/** Action handler for the "use" verb for entities that provide a use function. */
	virtual void OnActionItemUse() = 0;

	/** Action handler for picking up an item / entity. */
	virtual void OnActionItemPickup() = 0;

	/** Action handler for dropping an item / entity. */
	virtual void OnActionItemDrop() = 0;


	/**
	Action handler for throwing an item. General expectation is this will be used for "Pick and Throw" weapons but it
	might need to also handle regular weapons and even ordinary items.
	**/
	virtual void OnActionItemToss() = 0;

	/**
	An action bar entry has been triggered. Take whatever action is appropriate.

	\param	actionBarId Identifier for the action bar.
	**/
	virtual void OnActionBarUse(int actionBarId) = 0;

	/**
	A function bar entry has been triggered. Take whatever action is appropriate.

	\param	actionBarId Identifier for the action bar.
	**/
	virtual void OnFunctionBarUse(int actionBarId) = 0;

	/** Received when the player has indicated the actor should enter "inspection mode". */
	virtual void OnActionInspectStart() = 0;

	/** Received when the player has indicated the actor should try and inspect a nearby entity. */
	virtual void OnActionInspect() = 0;

	/** Received when the player has indicated the actor should exit "inspection mode". */
	virtual void OnActionInspectEnd() = 0;

	/** Received when the player has indicated the actor should enter "interaction mode". */
	virtual void OnActionInteractionStart() = 0;

	/** Received when the player has indicated the actor should try and interact with a nearby entity. */
	virtual void OnActionInteractionTick() = 0;

	/** Received when the player has indicated the actor should exit "interaction mode". */
	virtual void OnActionInteractionEnd() = 0;

	/** Executes the action crouch toggle action. */
	virtual void OnActionCrouchToggle() = 0;

	/** Executes the action crawl toggle action. */
	virtual void OnActionCrawlToggle() = 0;

	/** Executes the action kneel toggle action. */
	virtual void OnActionKneelToggle() = 0;

	/** Executes the action sit toggle action. */
	virtual void OnActionSitToggle() = 0;

	/** Handle requests to toggle between walking and jogging. */
	virtual void OnActionJogToggle() = 0;

	/** Handle requests to begin sprinting. */
	virtual void OnActionSprintStart() = 0;

	/** Handle requests to stop sprinting. */
	virtual void OnActionSprintStop() = 0;

	/** Is the actor currently sprinting?  */
	virtual bool IsSprinting() const = 0;

	/** Is the actor currently jogging?  */
	virtual bool IsJogging() const = 0;


	/**
	Call this to place the actor into "interaction mode". This should be called by any section of code that wants to
	trigger an interaction with the actor. This will lock out any other attempts to start an interaction until this one
	has completed. Generally, you will want whatever process is kicked off by the 'OnActionInteractionStart' function to
	make a call to this at the start of it's specific interaction.

	\param [in,out]	pInteraction If non-null, the interaction.
	**/
	virtual void InteractionStart(IInteraction* pInteraction) = 0;


	/**
	Received at intervals during an on-going interaction.

	\param [in,out]	pInteraction If non-null, the interaction.
	**/
	virtual void InteractionTick(IInteraction* pInteraction) = 0;


	/**
	Call this to remove the actor from "interaction mode". This will open the actor up to accepting interactions
	again.

	\param [in,out]	pInteraction If non-null, the interaction.
	**/
	virtual void InteractionEnd(IInteraction* pInteraction) = 0;


	/**
	Queue an action onto the animation queue.

	\param [in,out]	pAction The action.
	**/
	virtual void QueueAction(IAction& pAction) = 0;


	/**
	Gets action controller.

	\return Null if it fails, else the action controller.
	**/
	virtual IActionController* GetActionController() const = 0;

	virtual const SActorMannequinParams* GetMannequinParams() const = 0;

	/**
	Call this routine when a player has attached to this actor to complete the circle. The player is now in control
	of this actor's movements and may view them through a camera attached to it. This routine is not meant for direct
	calling, instead use the AttachToCharacter routine that is provided on a valid CPlayerComponent object.

	\param [in,out]	player The player.
	**/
	virtual void OnPlayerAttach(CPlayerComponent& player) = 0;


	/**
	Detach the player from this character. This needs to be called when a player is no longer controlling or attached
	to this actor.
	**/
	virtual void OnPlayerDetach() = 0;


	/** The actor has toggled between third person and first person view modes. */
	virtual void OnToggleFirstPerson() = 0;


	/** Kill the actor. */
	virtual void OnKill() = 0;


	/** The actor has died. */
	virtual void OnDeath() = 0;


	/** The actor has been revived. */
	virtual void OnRevive() = 0;


	/** Executes the jump action. */
	virtual void OnJump() = 0;


	/**
	The actor has entered a vehicle.

	\param	strVehicleClassName Name of the vehicle class.
	\param	strSeatName		    Name of the seat.
	\param	bThirdPerson	    true if we are in third person.
	**/
	virtual void OnEnterVehicle(const char* strVehicleClassName, const char* strSeatName, bool bThirdPerson) = 0;


	/** The actor has exited a vehicle. */
	virtual void OnExitVehicle() = 0;


	/**
	The actor's health has changed.

	\param	newHealth The new health.
	**/
	virtual void OnHealthChanged(float newHealth) = 0;


	/**
	The actor has picked up an item.

	\param	itemId Identifier for the item.
	**/
	virtual void OnItemPickedUp(EntityId itemId) = 0;


	/**
	The actor has used an item.

	\param	itemId Identifier for the item.
	**/
	virtual void OnItemUsed(EntityId itemId) = 0;


	/**
	The actor has dropped an item.

	\param	itemId Identifier for the item.
	**/
	virtual void OnItemDropped(EntityId itemId) = 0;


	/**
	The actor's sprint stamina has changed.

	\param	newStamina The new stamina.
	**/
	virtual void OnSprintStaminaChanged(float newStamina) = 0;
};
DECLARE_SHARED_POINTERS(IActor);

}