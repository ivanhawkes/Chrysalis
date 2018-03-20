#pragma once

#include <Actor/Fate.h>
#include <CryAISystem/IAgent.h>
#include "Snaplocks/Snaplock.h"
#include "DefaultComponents/Geometry/AdvancedAnimationComponent.h"
#include "DefaultComponents/Physics/CharacterControllerComponent.h"
#include <Components/Player/Input/PlayerInputComponent.h>
#include <Actor/ActorControllerComponent.h>
#include <Entities/Interaction/IEntityInteraction.h>


namespace Chrysalis
{
class CPlayerComponent;
class CActorControllerComponent;
class CEntityAwarenessComponent;
class CSnaplockComponent;
class CInventoryComponent;
class CEquipmentComponent;
struct IItem;


/** Define a set of snaplock types that will be used by character entities e.g. equipment slots **/
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_HEAD, "Actor Head", 0x617985533E2A4E5D, 0xB43CD7D2CFC60571)
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_FACE, "Actor Face", 0x10BA29FEA6F14F48, 0xAC475D386AD3637D)
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_NECK, "Actor Neck", 0x98CF570E51B343AE, 0xA566097392EEA395)
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_SHOULDERS, "Actor Shoulders", 0x0435E8CD9FA84215, 0x9FEEBDED107A3482)
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_CHEST, "Actor Chest", 0x7E232140AFC04AF0, 0x95A200219E4B5031)
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_BACK, "Actor Back", 0xDF40E37A38CB4530, 0xB421E09F4972A025)
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_LEFTARM, "Actor Left Arm", 0xB6B761D420F445CA, 0xB8195FCAC86B0E7D)
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_RIGHTARM, "Actor Right Arm", 0x5536709863DE489C, 0xA94FA3900B546241)
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_LEFTHAND, "Actor Left Hand", 0xA1EB42095D804E6B, 0xABDB1052EBC22704)
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_RIGHTHAND, "Actor Right Hand", 0xC9225FCED1E34228, 0xBA2B35347EB4E30D)
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_WAIST, "Actor Waist", 0x7631F1B70DC14B27, 0x9B3D14368B2EE2DE)
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_LEFTLEG, "Actor Left Leg", 0x6C0F6F7269504D25, 0x97F96FBCB54BA444)
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_RIGHTLEG, "Actor Right Leg", 0xB349158398FD4B19, 0xB0F42137CF694919)
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_LEFTFOOT, "Actor Left Foot", 0x8880849B8D5E4A28, 0xB18F39A02772920B)
DECLARE_SNAPLOCK_TYPE(SLT_ACTOR_RIGHTFOOT, "Actor Right Foot", 0xB3619CEF068F4388, 0xB49947F598CF321D)


/** Represents all of the available actor class types. */
enum EActorClassType
{
	///< Actor is a plain actor.
	EACT_ACTOR = 0,

	///< Actor is a player.
	EACT_PLAYER,

	///< Actor is a character.
	EACT_CHARACTER,

	///< Actor is a mount.
	EACT_MOUNT,

	///< Actor is a pet.
	EACT_PET,
};


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
	The actor's sprint stamina has changed.

	\param [in,out]	pActor If non-null, the actor.
	\param	newStamina	   The new stamina.
	**/
	virtual void OnSprintStaminaChanged(IActor* pActor, float newStamina) = 0;
};


/**
A very simple base class for all actors. This is replacing the old CryTek interface.

TODO: Remove this at some point, or strip it nearly bare.
**/
class IActorComponent
	: public IEntityComponent
{
protected:
	friend CChrysalisCorePlugin;

	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

public:
	IActorComponent() {}
	virtual ~IActorComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<IActorComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{AD23C7CE-E86B-4CDE-B585-32965C514F7D}"_cry_guid;
		return id;
	}


	/**	Is this actor being controller by the local player?
	TODO: We might not need this and IsClient, look at removing one or naming them better. */
	virtual const bool IsPlayer() const = 0;

	/**	Is this actor being controller by the local player? */
	virtual const bool IsClient() const = 0;

	/** Get's the player controlling this actor, if there is one. This may return nullptr. */
	virtual CPlayerComponent* GetPlayer() const = 0;

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
	virtual const Vec3 GetLocalEyePos() const { return { 0.0f, 0.0f, 1.82f }; };

	virtual Vec3 GetLocalLeftHandPos() const { return Vec3(ZERO); };

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
	**/
	virtual void InteractionStart(IInteraction* pInteraction) = 0;

	/** Received at intervals during an on-going interaction. */
	virtual void InteractionTick(IInteraction* pInteraction) = 0;

	/** Call this to remove the actor from "interaction mode". This will open the actor up to accepting interactions again. */
	virtual void InteractionEnd(IInteraction* pInteraction) = 0;

	/** Queue an action onto the animation queue. */
	virtual void QueueAction(TAction<SAnimationContext>& pAction) = 0;

	/**
	Gets action controller.

	\return Null if it fails, else the action controller.
	**/
	virtual IActionController* GetActionController() const = 0;

	/** Kill the character. */
	virtual void Kill() = 0;

	enum EReasonForRevive
	{
		RFR_None,
		RFR_FromInit,
		RFR_StartSpectating,
		RFR_Spawn,
		RFR_ScriptBind,
	};

	/**
	Revive the character from death.

	\param	reasonForRevive	the reason for revive.
	*/
	virtual void Revive(EReasonForRevive reasonForRevive = RFR_None) = 0;


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
};
DECLARE_SHARED_POINTERS(IActor);


/** Base class for any components that wish to provide actor services. */

// #TODO: probably needs to also implement IInventoryListener to listen for inventory changes.

class CActorComponent
	: public IActorComponent
	, public IActorEventListener
{
protected:
	friend CChrysalisCorePlugin;

	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_PREPHYSICSUPDATE); }
	// ~IEntityComponent

	virtual void Update(SEntityUpdateContext* pCtx);

public:
	CActorComponent() {};
	virtual ~CActorComponent();

	static void ReflectType(Schematyc::CTypeDesc<CActorComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{D11A58F0-09B5-4685-B5EA-CFC04AEFF2E7}"_cry_guid;
		return id;
	}


	// ***
	// *** IActorEventListener
	// ***

public:

	/**
	Executes the special move action. Special moves are defined in an enum.

	\param [in,out]	pActor If non-null, the actor.
	\param	move		   The move.
	**/

	// #TODO: Since there are just two special moves, consider making them full events instead.
	void OnSpecialMove(IActor* pActor, IActorEventListener::ESpecialMove move) override;


	/**
	The actor has died.

	\param [in,out]	pActor If non-null, the actor.
	\param	bIsGod		   true if this instance is god.
	**/
	void OnDeath(IActor* pActor, bool bIsGod) override;


	/**
	The actor has been revived.

	\param [in,out]	pActor If non-null, the actor.
	\param	bIsGod		   true if this instance is god.
	**/
	void OnRevive(IActor* pActor, bool bIsGod) override;


	/**
	The actor has entered a vehicle.

	\param [in,out]	pActor	    If non-null, the actor.
	\param	strVehicleClassName Name of the vehicle class.
	\param	strSeatName		    Name of the seat.
	\param	bThirdPerson	    true if we are in third person.
	**/
	void OnEnterVehicle(IActor* pActor, const char* strVehicleClassName, const char* strSeatName, bool bThirdPerson) override;


	/**
	The actor has exited a vehicle.

	\param [in,out]	pActor If non-null, the actor.
	**/
	void OnExitVehicle(IActor* pActor) override;


	/**
	The actor's health has changed.

	\param [in,out]	pActor If non-null, the actor.
	\param	newHealth	   The new health.
	**/
	void OnHealthChanged(IActor* pActor, float newHealth) override;


	/**
	The actor has picked up an item.

	\param [in,out]	pActor If non-null, the actor.
	\param	itemId		   Identifier for the item.
	**/
	void OnItemPickedUp(IActor* pActor, EntityId itemId) override;


	/**
	The actor has used an item.

	\param [in,out]	pActor If non-null, the actor.
	\param	itemId		   Identifier for the item.
	**/
	void OnItemUsed(IActor* pActor, EntityId itemId) override;


	/**
	The actor has dropped an item.

	\param [in,out]	pActor If non-null, the actor.
	\param	itemId		   Identifier for the item.
	**/
	void OnItemDropped(IActor* pActor, EntityId itemId) override;


	/**
	The actor's sprint stamina has changed.

	\param [in,out]	pActor If non-null, the actor.
	\param	newStamina	   The new stamina.
	**/
	void OnSprintStaminaChanged(IActor* pActor, float newStamina) override;

public:
	/** Provide a means for them to query the move velocity and direction. */
	const Vec3& GetVelocity() const { return m_pCharacterControllerComponent->GetVelocity(); }
	Vec3 GetMoveDirection() const { return m_pCharacterControllerComponent->GetMoveDirection(); }

	/**	Is this actor being controller by the local player? */
	const bool IsPlayer() const override;

	/**	Is this actor being controller by the local player? */
	const bool IsClient() const override;

	/** Get's the player controlling this actor, if there is one. This may return nullptr. */
	CPlayerComponent* GetPlayer() const override { return m_pPlayer; };

	/** The actor has toggled between third person and first person view modes. */
	void OnToggleFirstPerson() override;

	/**
	Gets the actors's pre-determined fate.

	\return	The fate.
	*/
	const CFate& GetFate() { return m_fate; }

	/** Gives you access to the controller component for this actor. Use with caution. **/
	CActorControllerComponent* GetControllerComponent() const { return m_pActorControllerComponent; }

	ICharacterInstance* GetCharacter() const override;

protected:
	Cry::DefaultComponents::CAdvancedAnimationComponent* m_pAdvancedAnimationComponent { nullptr };
	Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterControllerComponent { nullptr };

	Schematyc::CharacterFileName m_geometryFirstPerson;
	Schematyc::CharacterFileName m_geometryThirdPerson;

	/** Called to indicate the entity must reset itself. This is often done during PostInit() and
	additionally by the editor when you both enter and leave game mode. */
	virtual void OnResetState();

	void ResetMannequin();

private:
	/** An component which is used to discover entities near the actor. */
	CEntityAwarenessComponent * m_pAwareness { nullptr };

	/** A component that allows for management of snaplocks. */
	CSnaplockComponent* m_pSnaplockComponent { nullptr };

	/** Manage their inventory. */
	CInventoryComponent* m_pInventoryComponent { nullptr };

	/** Manage their equipment. */
	CEquipmentComponent* m_pEquipmentComponent { nullptr };

	/** All control is handled through an actor controller. */
	CActorControllerComponent* m_pActorControllerComponent { nullptr };

	/**	A dynamic response proxy. **/
	IEntityDynamicResponseComponent* m_pDrsComponent;

	/** If a player is controlling this character, this pointer will be valid. */
	CPlayerComponent* m_pPlayer { nullptr };

	/** The pre-determined fate for this actor. */
	CFate m_fate;


	// ***
	// *** AI / Player Control
	// ***

public:
	void OnPlayerAttach(CPlayerComponent& player) override;
	void OnPlayerDetach() override;


	// ***
	// *** Life-cycle
	// ***

public:
	/** Kill the character. */
	void Kill() override;


	/**
	Revive the character from death.

	\param	reasonForRevive	the reason for revive.
	*/
	void Revive(EReasonForRevive reasonForRevive = RFR_Spawn) override;


	// ***
	// *** Handle interactions with other entities.
	// *** TODO: This code feels a little loose, like it's API needs to be better defined.
	// ***

public:
	void OnActionItemUse() override;
	void OnActionItemPickup() override;
	void OnActionItemDrop() override;
	void OnActionItemToss() override;
	void OnActionBarUse(int actionBarId) override;
	void OnActionInspectStart() override;
	void OnActionInspect() override;
	void OnActionInspectEnd() override;
	void OnActionInteractionStart() override;
	void OnActionInteractionTick() override;
	void OnActionInteractionEnd() override;
	void OnActionCrouchToggle() override { m_pActorControllerComponent->OnActionCrouchToggle(); };
	void OnActionCrawlToggle() override { m_pActorControllerComponent->OnActionCrawlToggle(); };
	void OnActionKneelToggle() override { m_pActorControllerComponent->OnActionKneelToggle(); };
	void OnActionSitToggle() override { m_pActorControllerComponent->OnActionSitToggle(); };
	void OnActionJogToggle() override { m_pActorControllerComponent->OnActionJogToggle(); };
	void OnActionSprintStart() override { m_pActorControllerComponent->OnActionSprintStart(); };
	void OnActionSprintStop() override { m_pActorControllerComponent->OnActionSprintStop(); };
	bool IsSprinting() const override { return m_pActorControllerComponent->IsSprinting(); };
	bool IsJogging() const override { return m_pActorControllerComponent->IsJogging(); };

	/** Is the actor currently interacting with another entity? */
	bool IsInteracting() const { return m_pInteraction != nullptr; }

	/**
	Call this to place the actor into "interaction mode". This should be called by any section of code that wants to
	trigger an interaction with the actor. This will lock out any other attempts to start an interaction until this one
	has completed. Generally, you will want whatever process is kicked off by the 'OnActionInteractionStart' function to
	make a call to this at the start of it's specific interaction.
	**/
	void InteractionStart(IInteraction* pInteraction) override;

	/** Received at intervals during an on-going interaction. */
	void InteractionTick(IInteraction* pInteraction) override;

	/** Call this to remove the actor from "interaction mode". This will open the actor up to accepting interactions again. */
	void InteractionEnd(IInteraction* pInteraction) override;

private:
	/** If we are interacting with an entity, it is this entity. */
	EntityId m_interactionEntityId { INVALID_ENTITYID };

	/** If we're interacting with something, this is the actual interaction. */
	IInteractionPtr m_pInteraction;

	/** True when the actor is busy interaction with something, and shouldn't be allowed to start a new interaction until
	the first is finished. */
	bool isBusyInInteraction { false };

	// ***
	// *** Allow control of the actor's animations / fragments / etc.
	// ***

public:
	void QueueAction(TAction<SAnimationContext>& pAction) override { m_pAdvancedAnimationComponent->QueueAction(pAction); };

	virtual IActionController* GetActionController() const;

	const SActorMannequinParams* GetMannequinParams() const { return m_actorMannequinParams; }


	/**
	Gets stance tag identifier. The returned TagID is useful for driving animation.

	\param	actorStance The actor's stance.

	\return The stance tag identifier.
	**/
	TagID GetStanceTagId(EActorStance actorStance);


	/**
	Gets posture tag identifier. The returned TagID is useful for driving animation.

	\param	actorPosture The actor's posture.

	\return The posture tag identifier.
	**/
	TagID GetPostureTagId(EActorPosture actorPosture);

private:
	const SActorMannequinParams* m_actorMannequinParams { nullptr };
	class CProceduralContextAim* m_pProceduralContextAim { nullptr };
	class CProceduralContextLook* m_pProceduralContextLook { nullptr };
	IActionController* m_pActionController { nullptr };

	// ***
	// *** Item System.
	// ***

public:
	EntityId GetCurrentItemId(bool includeVehicle) const;

	/**
	Gets current item.

	\param	includeVehicle true to include, false to exclude the vehicles in the search.

	\return null if it fails, else the current item.

	\sa		GetCurrentItemId for obtaining the Id, since this is a more direct call into the item system.
	**/
	IItem* GetCurrentItem(bool includeVehicle = false) const;

public:
	const Vec3 GetLocalEyePos() const override;

	Vec3 GetLocalLeftHandPos() const override;

	Vec3 GetLocalRightHandPos() const override;


	/**
	Query if this instance is in first person view.

	\return True if this instance is controlled by the local player in a first person perspective, false for all other cases.
	**/
	virtual bool IsViewFirstPerson() const;


	/**
	Queries the entity for an actor component.

	\param	entityId Identifier for the entity.

	\return Null if it fails, else the actor.
	**/
	static CActorComponent* GetActor(EntityId entityId)
	{
		auto pActor = gEnv->pEntitySystem->GetEntity(entityId);
		if (pActor)
			return pActor->GetComponent<CActorComponent>();

		return nullptr;
	};
};
}