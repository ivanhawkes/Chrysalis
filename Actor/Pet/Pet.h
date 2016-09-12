#pragma once

#include <Actor/Actor.h>
#include <StateMachine/StateMachine.h>


/**
An implementation of the IActor interface. A CPet is an actor that represents a character within the game,
either an NPC or PC which can be controlled by a player.

Characters may have inventory.

\sa	CGameObjectExtensionHelper&lt;CPet, IActor&gt;
*/

// TODO: probably needs to also implement IInventoryListener to listen for inventory changes.

class CPet : public CActor
{
private:
	// Declaration of the state machine that controls character movement.
	//DECLARE_STATE_MACHINE(CPet, Movement);

public:

	CPet();
	virtual ~CPet();


	// ***
	// *** IGameObjectExtension
	// ***

	void GetMemoryUsage(ICrySizer *pSizer) const override;
	bool Init(IGameObject * pGameObject) override;
	void PostInit(IGameObject * pGameObject) override;
	bool ReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) override;
	void PostReloadExtension(IGameObject * pGameObject, const SEntitySpawnParams &params) override;
	void Release() override;
	void FullSerialize(TSerialize ser) override;
	bool NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags) override;
	void PostSerialize() override {};
	void SerializeSpawnInfo(TSerialize ser) override {};
	ISerializableInfoPtr GetSpawnInfo() override;
	void Update(SEntityUpdateContext& ctx, int updateSlot) override;
	void HandleEvent(const SGameObjectEvent& event) override;
	void ProcessEvent(SEntityEvent& event) override;

	// It is critical we override the event priority to ensure we handle the event before CAnimatedCharacter.
	virtual IComponent::ComponentEventPriority GetEventPriority(const int eventID) const override;


	// *** 
	// *** IActor
	// *** 

public:

	/* You must override CActor for correct behaviour. */
	const char* GetActorClassName() const override { return "CPet"; };

	/* You must override CActor for correct behaviour. */
	ActorClass GetActorClass() const override { return EActorClassType::EACT_PET; };


	// ***
	// *** CActor
	// ***

public:


	// ***
	// *** CPet
	// ***

public:


protected:

	/**
	Physicalies this instance.

	\return	True if physicalization was successful. False otherwise.
	*/
	bool Physicalize() override;


	/**
	Pre physics update.
	*/
	void PrePhysicsUpdate() override;


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
	void Reset() override;


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
