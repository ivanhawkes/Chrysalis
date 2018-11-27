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
	void ProcessEvent(const SEntityEvent& event) override;
	Cry::Entity::EntityEventMask GetEventMask() const override { return EventToMask(EEntityEvent::Update); }
	// ~IEntityComponent

public:
	CMountComponent() {}
	virtual ~CMountComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CMountComponent>& desc);

	/** Resets the character to an initial state. */
	virtual void OnResetState();
};
}