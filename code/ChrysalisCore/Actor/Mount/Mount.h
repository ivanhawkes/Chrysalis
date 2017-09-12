#pragma once


namespace Chrysalis
{
/**
A CMountComponent represents a mountable creature within the game.
**/
// TODO: probably needs to also implement IInventoryListener to listen for inventory changes.

class CMountComponent
	: public IEntityComponent
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_UPDATE); }
	// ~IEntityComponent

public:
	CMountComponent() {}
	virtual ~CMountComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CMountComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{19A8D24A-E321-4E28-BFD7-BD35CDF85C32}"_cry_guid;
		return id;
	}

	/** Resets the character to an initial state. */
	virtual void OnResetState();
};
}