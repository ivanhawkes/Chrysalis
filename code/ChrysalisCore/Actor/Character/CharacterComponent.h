#pragma once


namespace Chrysalis
{
struct CActorComponent;
class CCharacterAttributesComponent;

/**
A CCharacterComponent is an actor that represents a character within the game, either an NPC or PC which can be
controlled by a player.

Characters may have inventory.
**/
class CCharacterComponent
	: public IEntityComponent
{
protected:
	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(const SEntityEvent& event) override;
	Cry::Entity::EventFlags GetEventMask() const override { return EEntityEvent::Update; }
	// ~IEntityComponent

public:
	CCharacterComponent() {}
	virtual ~CCharacterComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CCharacterComponent>& desc);

	/** Resets the character to an initial state. */
	virtual void OnResetState();

	/** Manage their equipment. */
	CCharacterAttributesComponent* m_pCharacterAttributesComponent {nullptr};

	CActorComponent* m_pActorComponent {nullptr};
};
}