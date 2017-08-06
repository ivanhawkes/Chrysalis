#pragma once

#include <Actor/Actor.h>
#include <Actor/Character/Movement/CharacterRotation.h>
#include <StateMachine/StateMachine.h>


namespace Chrysalis
{
class CCharacterAttributesComponent;


/**
A CCharacterComponent is an actor that represents a character within the game, either an NPC or PC which can be
controlled by a player.

Characters may have inventory.
**/
class CCharacterComponent
	: public CActor
{
protected:
	// Declaration of the state machine that controls character movement.
	DECLARE_STATE_MACHINE(CCharacterComponent, Movement);

	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_PREPHYSICSUPDATE); }
	// ~IEntityComponent

	virtual void Update() override;

public:
	CCharacterComponent() {}
	virtual ~CCharacterComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CCharacterComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{33E4A852-B605-4DEB-881F-D3EC252A9EDB}"_cry_guid;
		return id;
	}


	// ***
	// *** CCharacterComponent
	// ***

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
	virtual void OnResetState();


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


	// ***
	// *** Misc
	// ***

public:

	/** Manage their equipment. */
	CCharacterAttributesComponent* m_pCharacterAttributesComponent { nullptr };
};
}