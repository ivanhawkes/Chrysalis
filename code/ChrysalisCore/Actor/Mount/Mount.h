#pragma once

#include <Actor/Actor.h>
#include <StateMachine/StateMachine.h>


namespace Chrysalis
{
/**
A CMountComponent represents a mountable creature within the game.
**/
// TODO: probably needs to also implement IInventoryListener to listen for inventory changes.

class CMountComponent
	: public CActor
{
protected:
	// Declaration of the state machine that controls character movement.
	//DECLARE_STATE_MACHINE(CMountComponent, Movement);

	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_PREPHYSICSUPDATE); }
	// ~IEntityComponent

	virtual void Update() override;

public:
	CMountComponent() {}
	virtual ~CMountComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CMountComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{19A8D24A-E321-4E28-BFD7-BD35CDF85C32}"_cry_guid;
		return id;
	}

protected:

	/**
	Physicalies this instance.

	\return	True if physicalization was successful. False otherwise.
	*/
	bool Physicalize() override;


	/**
	Pre physics update.
	*/
	virtual void PrePhysicsUpdate();


	/**
	Registers this instance for GameObject event notifications (will receive HandleEvent() calls).
	*/
	void RegisterEvents();


	/**
	Scripts are able to raise an event which is passed back to the c++ code though the ENTITY_EVENT_SCRIPT_EVENT.
	This method handles those events.

	\param	eventName	  	Name of the event.
	\param	eventValueType	Type of the event value.
	\param	pEventValue   	The event value.
	*/
	void OnScriptEvent(SEntityEvent& event);


	/** Whenever a property in the editor is changed, this function is called. */
	void OnEditorPropertyChanged();


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
};
}