#pragma once

#include <Actor/Actor.h>
#include <Actor/Character/Movement/CharacterRotation.h>
#include <StateMachine/StateMachine.h>


/**
An implementation of the IActor interface. A CCharacter is an actor that represents a character within the game,
either an NPC or PC which can be controlled by a player.

Characters may have inventory.

\sa	CGameObjectExtensionHelper&lt;CCharacter, IActor&gt;
*/

// #TODO: probably needs to also implement IInventoryListener to listen for inventory changes.

class CCharacter : public CGameObjectExtensionHelper<CCharacter, CActor>, public IEntityPropertyGroup
//class CCharacter : public CActor, public IEntityPropertyGroup
{
	//CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CCharacter, "Character", 0xE03CE1AB90954702, 0xBFECD3E9E39F408B)

	// Declaration of the state machine that controls character movement.
	DECLARE_STATE_MACHINE(CCharacter, Movement);

public:

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_PREPHYSICSUPDATE); }
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	const char* GetLabel() const override { return "Character Properties"; };
	void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// ISimpleActor
	void PostInit(IGameObject * pGameObject) override;
	void PostSerialize() override {};
	void SerializeSpawnInfo(TSerialize ser) override {};
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	void HandleEvent(const SGameObjectEvent& event) override;

	// It is critical we override the event priority to ensure we handle the event before CAnimatedCharacter.
	virtual IEntityComponent::ComponentEventPriority GetEventPriority(const int eventID) const override;
	// ~IGameObjectExtension

	// Called to register the entity class and its properties
	static void Register();

	// IActor

	/* You must override CActor for correct behaviour. */
	const char* GetActorClassName() const override { return "CCharacter"; };

	/* You must override CActor for correct behaviour. */
	ActorClass GetActorClass() const override { return EActorClassType::EACT_ACTOR; };
	// ~IActor


	// ***
	// *** CCharacter
	// ***

public:

	CCharacter() {};
	virtual ~CCharacter() {};

protected:

	/**
	Physicalies this instance.

	\return	True if physicalization was successful. False otherwise.
	*/
	bool Physicalize() override;


	/**
	Pre physics update.
	*/
	void PrePhysicsUpdate();


	/**
	Registers this instance for GameObject event notifications (will receive HandleEvent() calls).
	*/
	void RegisterEvents();


	// ***
	// *** Life-cycle
	// ***

public:

	/** Resets the character to an initial state. */
	void OnResetState() override;


	/** Kill the character. */
	void Kill() override;


	/**
	Revive the character from death.

	\param	reasonForRevive	the reason for revive.
	*/
	void Revive(EReasonForRevive reasonForRevive = RFR_Spawn) override;


	// ***
	// *** Hierarchical State Machine Support - this allows us to abstract the HSM away from the base
	// *** actor class - giving the flexibility to use different state machines for different
	// *** derived classes e.g. mounts
	// ***

public:

	/**
	Select movement hierarchy for our HSM.
	*/
	void SelectMovementHierarchy() override;

	/** Release the HSM. */
	void MovementHSMRelease() override;

	/** Init the HSM. */
	void MovementHSMInit() override;

	/** Serialize the HSM. */
	void MovementHSMSerialize(TSerialize ser) override;

	/** Update the HSM. */
	void MovementHSMUpdate(SEntityUpdateContext& ctx, int updateSlot) override;

	/** Reset the HSM. */
	void MovementHSMReset() override;
};
