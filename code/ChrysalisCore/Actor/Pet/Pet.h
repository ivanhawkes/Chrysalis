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
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(const SEntityEvent& event) override;
	Cry::Entity::EntityEventMask GetEventMask() const override { return EventToMask(EEntityEvent::Update); }
	// ~IEntityComponent

public:
	CPetComponent() {}
	virtual ~CPetComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CPetComponent>& desc);

	/** Resets the character to an initial state. */
	virtual void OnResetState();
};
}