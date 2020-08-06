#pragma once

#include <Actor/Fate.h>
#include <CryAISystem/IAgent.h>
#include "DefaultComponents/Physics/CharacterControllerComponent.h"
#include <Components/Actor/ActorControllerComponent.h>
#include <Components/Animation/ActorAnimationComponent.h>
#include <Components/Player/Input/PlayerInputComponent.h>
#include <Components/Inventory/InventoryComponent.h>
#include <Entities/Interaction/IEntityInteraction.h>
#include <Interfaces/IActor.h>
#include <Interfaces/ISpell.h>
#include <Snaplocks/Snaplock.h>


namespace Chrysalis
{
class CPlayerComponent;
class CActorControllerComponent;
class CEntityAwarenessComponent;
class CSnaplockComponent;
class CSpellbookComponent;
class CSpellParticipantComponent;
class CInventoryComponent;
class CEquipmentComponent;
class IItemReceipt;


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
	/** Actor is a plain actor. */
	EACT_ACTOR = 0,

	/** Actor is a player. */
	EACT_PLAYER,

	/** Actor is a character. */
	EACT_CHARACTER,

	/** Actor is a mount. */
	EACT_MOUNT,

	/** Actor is a pet. */
	EACT_PET,
};


/** Base class for any components that wish to provide actor services. */

struct CActorComponent
	: public IEntityComponent, public IActor, public ISpellParticipant, public CPlayerInputComponent::IInputEventListener
{
protected:
	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(const SEntityEvent& event) override;
	Cry::Entity::EventFlags GetEventMask() const override { return EEntityEvent::Initialize | EEntityEvent::Update | EEntityEvent::Remove; }
	// ~IEntityComponent

	// ISpellParticipant
	const entt::entity GetECSEntity() const override { return m_ecsEntity; };
	// ~ISpellParticipant

	// CPlayerInputComponent::IInputEventListener
	virtual void OnInputEscape(int activationMode) override;
	virtual void OnInputInteraction(int activationMode) override;
	virtual void OnInputActionBarUse(int activationMode, int buttonId) override;
	virtual void OnInputFunctionBarUse(int activationMode, int buttonId) override;
	// ~CPlayerInputComponent::IInputEventListener


	virtual void Update(SEntityUpdateContext* pCtx);

public:
	CActorComponent() {};
	virtual ~CActorComponent() {};

	static void ReflectType(Schematyc::CTypeDesc<CActorComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{D11A58F0-09B5-4685-B5EA-CFC04AEFF2E7}"_cry_guid;
		return id;
	}


	/** Kill the actor. */
	void OnKill() override;


	/** The actor has died. */
	void OnDeath() override;


	/** The actor has been revived. */
	void OnRevive() override;


	/** Executes the jump action. */
	void OnJump() override;


	/**
	The actor has entered a vehicle.

	\param	strVehicleClassName Name of the vehicle class.
	\param	strSeatName		    Name of the seat.
	\param	bThirdPerson	    true if we are in third person.
	**/
	void OnEnterVehicle(const char* strVehicleClassName, const char* strSeatName, bool bThirdPerson) override;


	/** The actor has exited a vehicle. */
	void OnExitVehicle() override;


	/**
	The actor's health has changed.

	\param	newHealth The new health.
	**/
	void OnHealthChanged(float newHealth) override;


	/**
	The actor has picked up an item.

	\param	itemId Identifier for the item.
	**/
	void OnItemPickedUp(EntityId itemId) override;


	/**
	The actor has used an item.

	\param	itemId Identifier for the item.
	**/
	void OnItemUsed(EntityId itemId) override;


	/**
	The actor has dropped an item.

	\param	itemId Identifier for the item.
	**/
	void OnItemDropped(EntityId itemId) override;


	/**
	The actor's sprint stamina has changed.

	\param	newStamina The new stamina.
	**/
	void OnSprintStaminaChanged(float newStamina) override;

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
	CActorAnimationComponent* m_pActorAnimationComponent {nullptr};
	Cry::DefaultComponents::CCharacterControllerComponent* m_pCharacterControllerComponent {nullptr};

	Schematyc::CharacterFileName m_geometryFirstPerson;
	Schematyc::CharacterFileName m_geometryThirdPerson;

	/** Called to indicate the entity must reset itself. This is often done during PostInit() and
	additionally by the editor when you both enter and leave game mode. */
	virtual void OnResetState();

	/** Set the IK bones, if applicable. */
	void SetIK() const;

	/** Set the look IK on or off, giving it a target if it's being set on. **/
	bool SetLookingIK(const bool isLooking, const Vec3& lookTarget) const;

private:
	/** An component which is used to discover entities near the actor. */
	CEntityAwarenessComponent* m_pAwarenessComponent {nullptr};

	/** All control is handled through an actor controller. */
	CActorControllerComponent* m_pActorControllerComponent {nullptr};

	/** A spellbook allows an actor to access spells and to offer spells to others which can be cast. */
	CSpellbookComponent* m_pSpellbookComponent {nullptr};

	/** A spell participant component allows spells to be cast onto this entity. */
	CSpellParticipantComponent* m_pSpellParticipantComponent {nullptr};

	/** Manage their inventory. */
	CInventoryComponent* m_pInventoryComponent {nullptr};

	/** Manage their equipment. */
	CEquipmentComponent* m_pEquipmentComponent {nullptr};

	/**	A dynamic response proxy. **/
	IEntityDynamicResponseComponent* m_pDrsComponent {nullptr};

	/** A component that allows for management of snaplocks. */
	CSnaplockComponent* m_pSnaplockComponent {nullptr};

	/** If a player is controlling this character, this pointer will be valid. */
	CPlayerComponent* m_pPlayer {nullptr};

	/** The pre-determined fate for this actor. */
	CFate m_fate;

	/** The identifier for this spell participant in the ECS. */
	entt::entity m_ecsEntity {entt::null};

	// ***
	// *** AI / Player Control
	// ***

public:
	void OnPlayerAttach(CPlayerComponent& player) override;
	void OnPlayerDetach() override;


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
	void OnFunctionBarUse(int actionBarId) override;

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
	EntityId m_interactionEntityId {INVALID_ENTITYID};

	/** If we're interacting with something, this is the actual interaction. */
	IInteractionPtr m_pInteraction;

	/** True when the actor is busy interaction with something, and shouldn't be allowed to start a new interaction until
	the first is finished. */
	bool isBusyInInteraction {false};

	// ***
	// *** Allow control of the actor's animations / fragments / etc.
	// ***

public:
	void QueueAction(IAction& pAction) override { m_pActorAnimationComponent->QueueAction(pAction); };

	/**
	Get access to the action controller. Try to limit calls to this function as it's only here since some
	functions need the controller passed into them.

	\return The action controller.
	**/
	virtual IActionController* GetActionController() const
	{
		return m_pActorAnimationComponent->GetActionController();
	}

	const SActorMannequinParams* GetMannequinParams() const override { return m_actorMannequinParams; }


	/**
	Gets stance tag identifier. The returned TagID is useful for driving animation.

	\param	actorStance The actor's stance.

	\return The stance tag identifier.
	**/
	TagID GetStanceTagId(EActorStance actorStance) const;


	/**
	Gets posture tag identifier. The returned TagID is useful for driving animation.

	\param	actorPosture The actor's posture.

	\return The posture tag identifier.
	**/
	TagID GetPostureTagId(EActorPosture actorPosture) const;

private:
	const SActorMannequinParams* m_actorMannequinParams {nullptr};
	class CProceduralContextAim* m_pProceduralContextAim {nullptr};
	class CProceduralContextLook* m_pProceduralContextLook {nullptr};

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
	IItemReceipt* GetCurrentItem(bool includeVehicle = false) const;

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