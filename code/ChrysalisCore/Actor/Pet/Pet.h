#pragma once

#include <Actor/Actor.h>
#include <StateMachine/StateMachine.h>


namespace Chrysalis
{
/**
A CPetComponent is an actor that represents a pet within the game.
**/
// TODO: probably needs to also implement IInventoryListener to listen for inventory changes.

class CPetComponent
	: public IEntityComponent
{
protected:
	// Declaration of the state machine that controls character movement.
	//DECLARE_STATE_MACHINE(CPetComponent, Movement);

	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_PREPHYSICSUPDATE); }
	// ~IEntityComponent

	/** Pre physics update. */
	virtual void PrePhysicsUpdate();

public:
	CPetComponent() {}
	virtual ~CPetComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CPetComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{03DCFDAE-6F31-41D3-9FF5-26F79BB04278}"_cry_guid;
		return id;
	}

	/** Resets the character to an initial state. */
	virtual void OnResetState();

	//// ***
	//// *** Hierarchical State Machine Support - this allows us to abstract the HSM away from the base
	//// *** actor class - giving the flexibility to use different state machines for different
	//// *** derived classes e.g. mounts
	//// ***

	///** Select movement hierarchy for our HSM. */
	//void SelectMovementHierarchy() override;

	///** Release the HSM. */
	//void MovementHSMRelease() override;

	///** Init the HSM. */
	//void MovementHSMInit() override;

	///** Serialize the HSM. */
	//void MovementHSMSerialize(TSerialize ser) override;

	///** Update the HSM. */
	//void MovementHSMUpdate(SEntityUpdateContext& ctx, int updateSlot) override;

	///** Reset the HSM. */
	//void MovementHSMReset() override;
};
}