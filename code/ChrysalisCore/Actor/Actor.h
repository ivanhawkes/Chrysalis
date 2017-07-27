#pragma once

#include <IAnimatedCharacter.h>
#include <Actor/ISimpleActor.h>
#include <Actor/Fate.h>
#include <Actor/ActorState.h>
#include <Actor/ActorPhysics.h>
#include <Actor/ActorStance.h>
#include <Actor/IActorEventListener.h>
#include <CryAISystem/IAgent.h>
#include "Snaplocks/Snaplock.h"
//#include <Actor/Character/Movement/CharacterRotation.h>

class CPlayer;
class CActorMovementController;
class IActionController;
struct SAnimationContext;
struct SActorMovementRequest;
class CEntityAwarenessComponent;
class CSnaplockComponent;
class CInventoryComponent;
class CEquipmentComponent;


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
An implementation of the IActor interface. A CActor is an actor that represents a character within the game,
either an NPC or PC which can be controlled by a player.

Characters may have inventory.

\sa	CGameObjectExtensionHelper&lt;CActor, IActor&gt;
*/

// #TODO: probably needs to also implement IInventoryListener to listen for inventory changes.

class CActor : public CGameObjectExtensionHelper<CActor, ISimpleActor, 40>, public IActorEventListener
{
public:

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_PREPHYSICSUPDATE); }
	//struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	enum EProperties
	{
		// Animation.
		eProperty_Model = 0,
		eProperty_Controller_Definition,
		eProperty_Scope_Context,
		eProperty_Animation_Database,

		// Physics.
		eProperty_Mass,

		eNumProperties
	};


	CActor();
	virtual ~CActor();

	// ISimpleActor
	bool Init(IGameObject * pGameObject) override;
	void PostInit(IGameObject * pGameObject) override;
	void FullSerialize(TSerialize ser) override;
	void PostSerialize() override {};
	void SerializeSpawnInfo(TSerialize ser) override {};
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	bool IsThirdPerson() const override { return m_isThirdPerson; }

	// It is critical we override the event priority to ensure we handle the event before CAnimatedCharacter.
	// FIX: 5.4
	virtual IEntityComponent::ComponentEventPriority GetEventPriority(const int eventID) const override;

	int GetTeamId() const override { return m_teamId; };


	/**
	Gets this instance's local-space eye position (for a human, this is typically Vec3 (0, 0, 1.76f)).

	The code will first attempt to return a "#camera" helper if there is one. If only one eye is available (left_eye,
	right_eye) then that is used as the local position. In the case of two eyes, the position is the average of the two
	eyes.

	Position is calculated each time using the attachment manager, so it will be better to cache the results if you need
	to call this a few times in an update.

	\return This instance's local-space eye position.
	**/
	Vec3 GetLocalEyePos() const override;

	EntityId GetCurrentItemId(bool includeVehicle) const;

	/**
	Gets current item.

	\param	includeVehicle true to include, false to exclude the vehicles in the search.

	\return null if it fails, else the current item.

	\sa		GetCurrentItemId for obtaining the Id, since this is a more direct call into the item system.
	**/
	IItem* GetCurrentItem(bool includeVehicle = false) const override;



	/**
	Gets this instance's movement controller (the class instance that controls how this instance moves around).

	\return	This instance's movement controller.
	*/
	IMovementController* GetMovementController() const override;


	/**
	Query if this object is attached to a player. This does not have to be the local player, just as long as a player
	is attached.

	WARNING! Older code might try and cast this object to a player after receiving a true result here. Need to weed
	that code out and replace with another method.

	\return	true if player, false if not.
	*/
	bool IsPlayer() const override;


	/**
	Gets whether this instance is the "client actor" or not.

	\return	Whether this instance is the "client actor" or not.
	*/
	bool IsClient() const override;

	/**
	Gets the name of this instance's actor class (e.g. "CCharacter").

	NOTE: This must be overridden in derived classes.

	\return The name of this instance's actor class.
	**/
	const char* GetActorClassName() const override { return "CActor"; };


	/**
	Gets this instance's actor class type.

	NOTE: This must be overridden in derived classes.

	/sa EActorClassType.

	\return	This instance's actor class type.
	*/
	ActorClass GetActorClass() const override { return EActorClassType::EACT_ACTOR; };


	/**
	Gets the animated character of this instance.

	\return	The animated character of this instance.
	*/
	IAnimatedCharacter* GetAnimatedCharacter() override { return m_pAnimatedCharacter; }


	/**
	Gets the non-modifiable version of the animated character of this instance.

	\return	The non-modifiable version of the animated character of this instance.
	*/
	const IAnimatedCharacter* GetAnimatedCharacter() const override { return m_pAnimatedCharacter; }


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
	The actor has toggled between third person and first person view modes.

	\param [in,out]	pActor If non-null, the actor.
	\param	bThirdPerson   true to third person.
	**/
	void OnToggleThirdPerson(IActor* pActor, bool isThirdPerson) override;


	/**
	The actor's sprint stamina has changed.

	\param [in,out]	pActor If non-null, the actor.
	\param	newStamina	   The new stamina.
	**/
	void OnSprintStaminaChanged(IActor* pActor, float newStamina) override;

private:
	bool m_isThirdPerson { false };

	// ***
	// *** CActor
	// ***

public:

	/**
	Gets actor statistics.

	\return	null if it fails, else the actor statistics.
	*/
	SActorState* GetActorState() { return &m_actorState; };


	/**
	Gets actor statistics.

	\return	null if it fails, else the actor statistics.
	*/
	const SActorState* GetActorState() const { return &m_actorState; };


	/**
	Gets actor physics.

	\return	null if it fails, else the actor physics.
	*/
	SActorPhysics* GetActorPhysics() { return &m_actorPhysics; };


	/**
	Gets actor physics.

	\return	null if it fails, else the actor physics.
	*/
	const SActorPhysics* GetActorPhysics() const { return &m_actorPhysics; };


	/**
	Provides access to our movement request object.

	This is used extensively by the movement state machine to store state as it's calculated / updated. Some states will
	make changes to the move request object e.g. flying and ladders. Most changes are enacted by the ground movement
	state, as you would generally expect. It's final state will be fed into the FinalizeMovementRequest function to pass
	the requested movement into the animation system.

	\return The move request.
	**/
	ILINE SCharacterMoveRequest& GetMoveRequest() { return m_moveRequest; }


	/**
	Gets the actors's pre-determined fate.

	\return	The fate.
	*/
	ILINE const CFate& GetFate() { return m_fate; }


	// HACK: remove this in favour of a more cohesive approach.
	bool GetMovingLastFrame() const { return m_wasMovingLastFrame; }


	/**
	Gets action controller.

	\return Null if there is no action controller, else the action controller.
	**/
	IActionController* GetActionController() const { return m_pActionController; }


	/**
	Queries the actor system for the entity.
	
	\param	entityId Identifier for the entity.
	
	\return Null if it fails, else the actor.
	**/
	static CActor* GetActor(EntityId entityId) { return static_cast<CActor*>(gEnv->pGameFramework->GetIActorSystem()->GetActor(entityId)); };

protected:
	string m_geometryFirstPerson;
	string m_geometryThirdPerson;
	float m_mass { 82.0f };
	string m_controllerDefinition { "human_male_controller_defs.xml" };
	string m_animationDatabase { "human_male.adb" };

	/** Called to indicate the entity must reset itself. This is often done during PostInit() and
	additionally by the editor when you both enter and leave game mode. */
	virtual void OnResetState();

	/**
	Physicalies this instance.

	\return	True if physicalization was successful. False otherwise.
	*/
	virtual bool Physicalize();


	/**
	Updates to the animation state described by frameMovementParams.

	\param	frameMovementParams	A variable-length parameters list containing frame movement parameters.
	*/
	virtual void UpdateAnimationState(const SActorMovementRequest& movementRequest);


	/** The movement controller. */
	CActorMovementController* m_pMovementController { nullptr };

	/** The actor physics. */
	SActorPhysics m_actorPhysics;

private:
	/** Specifies whether this instance is the client actor. */
	bool m_isClient { false };

	/** The actor's inventory. */
	IInventory* m_pInventory { nullptr };

	/** The actor's animated character. */
	IAnimatedCharacter* m_pAnimatedCharacter { nullptr };

	/** The action controller. */
	IActionController* m_pActionController { nullptr };

	/** Context for the animation. */
	SAnimationContext* m_pAnimationContext { nullptr };

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

	// #HACK: to test switching movement and idle fragments. Should query physics instead.
	// Keeping the actions here allows me to stop them, which is good for testing, but wrong in
	// so many ways.
	bool m_wasMovingLastFrame { false };

	/** Identifier for the team. */
	int m_teamId { 0 };

	/** If a player is controlling this character, this pointer will be valid. */
	CPlayer* m_pAttachedPlayer { nullptr };

	/** The current state for a character. This is shared by a lot of the state machine code. */
	SActorState m_actorState;

	/** The move request keeps track of how we wish to move this character based on input, state machine, and movement controllers. */
	SCharacterMoveRequest m_moveRequest {};

	/** A class that assists in working out animated character rotation. */
	//CCharacterRotation* m_characterRotation;

	/** The pre-determined fate for this actor. */
	CFate m_fate;


	// ***
	// *** AI / Player Control
	// ***

public:
	/**
	Query if this object is controlled by AI. This doesn't imply that there is no player for this character, simply
	that they are under AI control at present.

	\return	true if AI controlled, false if not.
	*/
	ILINE bool IsAIControlled() const { return m_isAIControlled; };


	/**
	Call this routine when a player has attached to this character to complete the circle. The player is now in
	control of this character's movements and may view them through a camera attached to this character. This routine is
	not meant for direct calling, instead use the AttachToCharacter routine that is provided on a valid CPlayer object.

	\param [in,out]	player	The player.
	*/
	void OnPlayerAttach(CPlayer& player);


	/**
	Detach the player from this character. This needs to be called when a player is no longer
	controlling or attached to this character.
	*/
	void OnPlayerDetach();

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
	enum EReasonForRevive
	{
		RFR_FromInit,
		RFR_StartSpectating,
		RFR_Spawn,
		RFR_ScriptBind,
	};


	/** Kill the character. */
	virtual void Kill();


	/**
	Revive the character from death.

	\param	reasonForRevive	the reason for revive.
	*/
	virtual void Revive(EReasonForRevive reasonForRevive = RFR_Spawn);


	virtual void ResetMannequin();


	// ***
	// *** Hierarchical State Machine Support - this allows us to abstract the HSM away from the base
	// *** actor class - giving the flexibility to use different state machines for different
	// *** derived classes e.g. mounts
	// ***

public:

	/**
	Select movement hierarchy for our HSM.
	*/
	virtual void SelectMovementHierarchy() = 0;

	/** Release the HSM. */
	// #TODO: Why does this need an empty implementation? If fails to build if I don't provide one.
	virtual void MovementHSMRelease() {};

	/** Init the HSM. */
	virtual void MovementHSMInit() = 0;

	/** Serialize the HSM. */
	virtual void MovementHSMSerialize(TSerialize ser) = 0;

	/** Update the HSM. */
	virtual void MovementHSMUpdate(SEntityUpdateContext& ctx, int updateSlot) = 0;

	/** Reset the HSM. */
	virtual void MovementHSMReset() = 0;


	// ***
	// *** Cut-scenes
	// ***
	// *** While cut-scenes are played for the local player, the requests are likely to also be made on behalf
	// *** of characters. In such cases, we pass along the request to the attached player if there is one.
	// ***

public:
	/** A cut-scene has been triggered for this character. */
	void OnBeginCutScene() {};


	/** A cut-scene has finished playing or been cancelled for this character. */
	void OnEndCutScene() {};


	// ***
	// *** Handle interactions with other entities.
	// *** TODO: This code feels a little loose, like it's API needs to be better defined.
	// ***

public:
	/**
	Action handler for the "use" verb for entities that provide a use function.

	\param	playerId	The entityId for the player who invoked this action.
	*/
	virtual void OnActionItemUse(EntityId playerId);


	/**
	Action handler for picking up an item / entity.

	\param	playerId	The entityId for the player who invoked this action.
	*/
	virtual void OnActionItemPickup(EntityId playerId);


	/**
	Action handler for dropping an item / entity.

	\param	playerId	The entityId for the player who invoked this action.
	*/
	virtual void OnActionItemDrop(EntityId playerId);


	/**
	Action handler for throwing an item. General expectation is this will be used for "Pick and Throw" weapons but it
	might need to also handle regular weapons and even ordinary items.

	\param	playerId	The entityId for the player who invoked this action.
	*/
	virtual void OnActionItemToss(EntityId playerId);


	/**
	An action bar entry has been triggered. Take whatever action is appropriate.

	\param	playerId    Identifier for the player.
	\param	actionBarId Identifier for the action bar.
	**/
	void OnActionBarUse(EntityId playerId, int actionBarId);


	/**
	Received when the player has indicated the actor should enter "inspection mode".

	\param	playerId	The entityId for the player who invoked this action.
	*/
	void OnActionInspectStart(EntityId playerId);


	/**
	Received when the player has indicated the actor should try and inspect a nearby entity.

	\param	playerId	The entityId for the player who invoked this action.
	*/
	void OnActionInspect(EntityId playerId);


	/**
	Received when the player has indicated the actor should exit "inspection mode".

	\param	playerId	The entityId for the player who invoked this action.
	*/
	void OnActionInspectEnd(EntityId playerId);


	/**
	Received when the player has indicated the actor should enter "interaction mode".

	\param	playerId	The entityId for the player who invoked this action.
	*/
	void OnActionInteractionStart(EntityId playerId);


	/**
	Received when the player has indicated the actor should try and interact with a nearby entity.

	\param	playerId	The entityId for the player who invoked this action.
	*/
	void OnActionInteraction(EntityId playerId);


	/**
	Received when the player has indicated the actor should exit "interaction mode".

	\param	playerId	The entityId for the player who invoked this action.
	*/
	void OnActionInteractionEnd(EntityId playerId);


	void OnActionCrouchToggle(EntityId playerId);


	void OnActionCrawlToggle(EntityId playerId);


	void OnActionKneelToggle(EntityId playerId);


	void OnActionSitToggle(EntityId playerId);

private:
	/** If we are interacting with an entity, it is this entity. */
	EntityId m_interactionEntityId { INVALID_ENTITYID };


	// ***
	// *** Movement Requests
	// ***

public:

	/**
	Handle requests to toggle between walking and jogging.

	\param	playerId	The entityId for the player who invoked this action.
	*/
	virtual void OnActionJogToggle(EntityId playerId);


	/**
	Handle requests to begin sprinting.

	\param	playerId	The entityId for the player who invoked this action.
	*/
	virtual void OnActionSprintStart(EntityId playerId);


	/**
	Handle requests to stop sprinting.

	\param	playerId	The entityId for the player who invoked this action.
	*/
	virtual void OnActionSprintStop(EntityId playerId);


	/** Is the actor currently sprinting?  */
	bool IsSprinting() const { return m_isSprinting; }

	/** Is the actor currently jogging?  */
	bool IsJogging() const { return m_isJogging; }


	/**
	Return a base movement speed for this actor, given the direction (local to character) in which they are moving.
	This allows for slower speeds when moving backwards or sideways. It doesn't take into account slope or terrain.

	\param	movementStateFlags The movement state flags.

	\return The movement base speed.
	**/
	float GetMovementBaseSpeed(uint32 movementStateFlags) const;

private:
	/** The actor is sprinting. */
	bool m_isSprinting { false };

	/** The actor is jogging. */
	bool m_isJogging { false };


	// ***
	// *** Misc
	// ***

public:
	virtual Vec3 GetLocalLeftHandPos() const;

	virtual Vec3 GetLocalRightHandPos() const;
};
