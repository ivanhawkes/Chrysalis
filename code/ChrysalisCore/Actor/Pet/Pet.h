#pragma once


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
	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(const SEntityEvent& event) override;
	Cry::Entity::EventFlags GetEventMask() const override { return EEntityEvent::Update; }
	// ~IEntityComponent

public:
	CPetComponent() {}
	virtual ~CPetComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CPetComponent>& desc);

	/** Resets the character to an initial state. */
	virtual void OnResetState();
};
}