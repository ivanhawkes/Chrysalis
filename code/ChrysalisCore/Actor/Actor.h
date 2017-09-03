#pragma once

#include <IAnimatedCharacter.h>
#include <Actor/Fate.h>
#include <Actor/ActorState.h>
#include <Actor/ActorPhysics.h>
#include <Actor/ActorStance.h>
#include <Actor/IActorEventListener.h>
#include <CryAISystem/IAgent.h>
#include "Snaplocks/Snaplock.h"
#include "DefaultComponents/Geometry/AdvancedAnimationComponent.h"
#include "DefaultComponents/Physics/CharacterControllerComponent.h"
#include <Components/Player/Input/IPlayerInputComponent.h>
#include <StateMachine/StateMachine.h>
//#include <Actor/Character/Movement/CharacterRotation.h>

class IActionController;
struct SAnimationContext;
struct SCharacterMoveRequest;


namespace Chrysalis
{
class CPlayerComponent;
class CActorMovementController;
class CEntityAwarenessComponent;
class CSnaplockComponent;
class CInventoryComponent;
class CEquipmentComponent;
struct IInventory;
struct SActorMovementRequest;


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


/**
A very simple base class for all actors. This is replacing the old CryTek interface.

TODO: Remove this at some point, or strip it nearly bare.
**/
class IActorComponent
	: public IEntityComponent
{
protected:
	friend CChrysalisCorePlugin;

	// Declaration of the state machine that controls character movement.
	DECLARE_STATE_MACHINE(IActorComponent, Movement);

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

	// TODO: CRITICAL: HACK: BROKEN: !!
	// Make this actually work.
	const bool IsPlayer() const { return true; };

	// TODO: CRITICAL: HACK: BROKEN: !!
	// Make this actually work.
	const bool IsClient() const { return true; };

	// TODO: CRITICAL: HACK: BROKEN: !!
	// Make this actually work.
	const virtual Vec3 GetLocalEyePos() const { return { 0.0f, 0.0f, 1.82f }; };

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
	virtual void OnActionInteraction() = 0;

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
	Return a base movement speed for this actor, given the direction (local to character) in which they are moving.
	This allows for slower speeds when moving backwards or sideways. It doesn't take into account slope or terrain.

	\param	movementDirectionFlags The movement state flags.

	\return The movement base speed.
	**/
	virtual float GetMovementBaseSpeed(TInputFlags movementDirectionFlags) const = 0;

	/** Kill the character. */
	virtual void Kill() = 0;

	enum EReasonForRevive
	{
		RFR_FromInit,
		RFR_StartSpectating,
		RFR_Spawn,
		RFR_ScriptBind,
	};

	/**
	Revive the character from death.

	\param	reasonForRevive	the reason for revive.
	*/
	virtual void Revive(EReasonForRevive reasonForRevive = RFR_Spawn) = 0;


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


	/**
	Query if this object is controlled by AI. This doesn't imply that there is no player for this actor, simply
	that they are under AI control at present.

	\return true if AI controlled, false if not.
	**/
	virtual bool IsAIControlled() const = 0;

	/** A cut-scene has been triggered for this character. */
	virtual void OnBeginCutScene() = 0;


	/** A cut-scene has finished playing or been cancelled for this character. */
	virtual void OnEndCutScene() = 0;


	/**
	Gets actor state.

	\return null if it fails, else the actor statistics.
	**/
	virtual SActorState* GetActorState() = 0;


	/**
	Gets actor state.

	\return	null if it fails, else the actor statistics.
	*/
	virtual const SActorState* GetActorState() const = 0;

	/**
	Provides access to our movement request object.

	This is used extensively by the movement state machine to store state as it's calculated / updated. Some states will
	make changes to the move request object e.g. flying and ladders. Most changes are enacted by the ground movement
	state, as you would generally expect. It's final state will be fed into the FinalizeMovementRequest function to pass
	the requested movement into the animation system.

	\return The move request.
	**/
	virtual SCharacterMoveRequest& GetMoveRequest() = 0;

	/** Executes the toggle third person action. */
	virtual void OnToggleThirdPerson() = 0;
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
	CActorComponent() {/*m_characterRotation = new CMountRotation(*this);*/ };
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
	The actor has changed stance.

	\param [in,out]	pActor If non-null, the actor.
	\param	stance		   The stance.
	**/
	void OnStanceChanged(IActor* pActor, EStance stance) override;


	/**
	The actor's sprint stamina has changed.

	\param [in,out]	pActor If non-null, the actor.
	\param	newStamina	   The new stamina.
	**/
	void OnSprintStaminaChanged(IActor* pActor, float newStamina) override;

private:
	bool m_isThirdPerson { false };

	// ***
	// *** CActorComponent
	// ***

public:

	/**
	Gets actor state.

	\return	null if it fails, else the actor statistics.
	*/
	SActorState* GetActorState() override { return &m_actorState; };


	/**
	Gets actor state.

	\return	null if it fails, else the actor statistics.
	*/
	const SActorState* GetActorState() const override { return &m_actorState; };


	/**
	Provides access to our movement request object.

	This is used extensively by the movement state machine to store state as it's calculated / updated. Some states will
	make changes to the move request object e.g. flying and ladders. Most changes are enacted by the ground movement
	state, as you would generally expect. It's final state will be fed into the FinalizeMovementRequest function to pass
	the requested movement into the animation system.

	\return The move request.
	**/
	SCharacterMoveRequest& GetMoveRequest() override { return m_moveRequest; }


	// Provide a way to access the character controller functions indirectly.
	virtual void AddVelocity(const Vec3& velocity) { m_pCharacterControllerComponent->AddVelocity(velocity); }
	virtual void SetVelocity(const Vec3& velocity) { m_pCharacterControllerComponent->SetVelocity(velocity); }
	const Vec3& GetVelocity() const { return m_pCharacterControllerComponent->GetVelocity(); }
	Vec3 GetMoveDirection() const { return m_pCharacterControllerComponent->GetMoveDirection(); }

	void OnToggleThirdPerson() override;

	/**
	Gets the actors's pre-determined fate.

	\return	The fate.
	*/
	const CFate& GetFate() { return m_fate; }

protected:
	Cry::DefaultComponents::CAdvancedAnimationComponent* m_pAdvancedAnimationComponent { nullptr };
	Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterControllerComponent { nullptr };

	Schematyc::CharacterFileName m_geometryFirstPerson;
	Schematyc::CharacterFileName m_geometryThirdPerson;

	/** Called to indicate the entity must reset itself. This is often done during PostInit() and
	additionally by the editor when you both enter and leave game mode. */
	virtual void OnResetState();

	virtual void UpdateMovementRequest(float frameTime);
	virtual void UpdateLookDirectionRequest(float frameTime);
	virtual void UpdateAnimation(float frameTime);

private:
	/** An component which is used to discover entities near the actor. */
	CEntityAwarenessComponent* m_pAwareness { nullptr };

	/** A component that allows for management of snaplocks. */
	CSnaplockComponent* m_pSnaplockComponent { nullptr };

	/** Manage their inventory. */
	CInventoryComponent* m_pInventoryComponent { nullptr };

	/** Manage their equipment. */
	CEquipmentComponent* m_pEquipmentComponent { nullptr };

	/**	A dynamic response proxy. **/
	IEntityDynamicResponseComponent* m_pDrsComponent;

	TagID m_rotateTagId;

	/*** A vector representing the direction and distance the player has requested this actor to move. */
	Vec3 m_movementRequest { ZERO };
	
	/** The direction the actor should be facing (pelvis) based on their movement inputs. */
	Quat m_lookOrientation { IDENTITY };

	/**	The yaw angular velocity - a measure of how quickly the actor is turning. **/
	float m_yawAngularVelocity { 0.0f };

	/** The continuous amount of time the actor has been receiving movement requests (seconds). */
	float m_movingDuration { 0.0f };

	/** If a player is controlling this character, this pointer will be valid. */
	CPlayerComponent* m_pPlayer { nullptr };

	/** The current state for a character. This is shared by a lot of the state machine code. */
	SActorState m_actorState;

	/** The move request keeps track of how we wish to move this character based on input, state machine, and movement controllers. */
	// TODO: CRITICAL: HACK: BROKEN: !!
	// This isn't being updated yet, so it will always return a default movement request.
	SCharacterMoveRequest m_moveRequest {};

	/** A class that assists in working out animated character rotation. */
	//CMountRotation* m_characterRotation;

	/** The pre-determined fate for this actor. */
	CFate m_fate;


	// ***
	// *** AI / Player Control
	// ***

public:
	bool IsAIControlled() const override { return m_isAIControlled; };
	void OnPlayerAttach(CPlayerComponent& player) override;
	void OnPlayerDetach() override;

private:
	/** true if this object is controlled by an AI. */
	bool m_isAIControlled;


	// ***
	// *** Stances
	// ***

public:
	EActorStance GetStance() const { return m_actorStance.GetStance(); }
	void SetStance(EActorStance stance) { m_actorStance.SetStance(stance); }
	EActorPosture GetPosture() const { return m_actorStance.GetPosture(); }
	void SetPosture(EActorPosture posture) { m_actorStance.SetPosture(posture); }

private:

	// Track and manage the stance the actor is in.
	CActorStance m_actorStance;


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


	//	// ***
	//	// *** Hierarchical State Machine Support - this allows us to abstract the HSM away from the base
	//	// *** actor class - giving the flexibility to use different state machines for different
	//	// *** derived classes e.g. mounts
	//	// ***
	//
	//public:
	//
	//	/**
	//	Select movement hierarchy for our HSM.
	//	*/
	//	virtual void SelectMovementHierarchy() = 0;
	//
	//	/** Release the HSM. */
	//	// #TODO: Why does this need an empty implementation? If fails to build if I don't provide one.
	//	virtual void MovementHSMRelease() {};
	//
	//	/** Init the HSM. */
	//	virtual void MovementHSMInit() = 0;
	//
	//	/** Serialize the HSM. */
	//	virtual void MovementHSMSerialize(TSerialize ser) = 0;
	//
	//	/** Update the HSM. */
	//	virtual void MovementHSMUpdate(SEntityUpdateContext& ctx, int updateSlot) = 0;
	//
	//	/** Reset the HSM. */
	//	virtual void MovementHSMReset() = 0;


		// ***
		// *** Cut-scenes
		// ***
		// *** While cut-scenes are played for the local player, the requests are likely to also be made on behalf
		// *** of characters. In such cases, we pass along the request to the attached player if there is one.
		// ***

public:
	/** A cut-scene has been triggered for this character. */
	void OnBeginCutScene() override {};

	/** A cut-scene has finished playing or been cancelled for this character. */
	void OnEndCutScene() override {};


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
	void OnActionInteraction() override;
	void OnActionInteractionEnd() override;
	void OnActionCrouchToggle() override;
	void OnActionCrawlToggle() override;
	void OnActionKneelToggle() override;
	void OnActionSitToggle() override;
	void OnActionJogToggle() override { m_isJogging = !m_isJogging; };
	void OnActionSprintStart() override { m_isSprinting = true; };
	void OnActionSprintStop() override { m_isSprinting = false; };
	bool IsSprinting() const override { return m_isSprinting; }
	bool IsJogging() const override { return m_isJogging; }
	float GetMovementBaseSpeed(TInputFlags movementDirectionFlags) const override;

private:
	/** If we are interacting with an entity, it is this entity. */
	EntityId m_interactionEntityId { INVALID_ENTITYID };

	/** The actor is sprinting. */
	bool m_isSprinting { false };

	/** The actor is jogging. */
	bool m_isJogging { false };


	// ***
	// *** Item System.
	// ***

	EntityId GetCurrentItemId(bool includeVehicle) const;

	/**
	Gets current item.

	\param	includeVehicle true to include, false to exclude the vehicles in the search.

	\return null if it fails, else the current item.

	\sa		GetCurrentItemId for obtaining the Id, since this is a more direct call into the item system.
	**/
	IItem* GetCurrentItem(bool includeVehicle = false) const;


	// ***
	// *** PREVIOUSLY REQUIRED TO BE IMPLEMENTED FOR IACTOR. THESE DEFINITIONS ARE STILL REQUIRED, BUT COULD NOW BE REFACTORED.
	// ***

public:
	/**
	Gets this instance's local-space eye position (for a human, this is typically Vec3 (0, 0, 1.76f)).

	The code will first attempt to return a "#camera" helper if there is one. If only one eye is available (left_eye,
	right_eye) then that is used as the local position. In the case of two eyes, the position is the average of the two
	eyes.

	Position is calculated each time using the attachment manager, so it will be better to cache the results if you need
	to call this a few times in an update.

	\return This instance's local-space eye position.
	**/
	const Vec3 GetLocalEyePos() const override;

	Vec3 GetLocalLeftHandPos() const override;

	Vec3 GetLocalRightHandPos() const override;

	virtual bool IsThirdPerson() const { return m_isThirdPerson; }


	/**
	Queries the entity for an actor component.

	\param	entityId Identifier for the entity.

	\return Null if it fails, else the actor.
	**/
	// TODO: CRITICAL: HACK: BROKEN: !! It's almost certain that this is not the desired functionality. Will this work
	// with inhereited classes? 
	static CActorComponent* GetActor(EntityId entityId) {
		auto pActor = gEnv->pEntitySystem->GetEntity(entityId);
		if (pActor)
			return pActor->GetComponent<CActorComponent>();

		return nullptr;
	};
};
}