#pragma once

#include "Entities/Interaction/IEntityInteraction.h"
#include <DefaultComponents/Geometry/StaticMeshComponent.h>

class Cry::DefaultComponents::CStaticMeshComponent;

namespace Chrysalis
{
class CSimpleAnimationComponent;
class CLockableComponent;
class CEntityInteractionComponent;

/**
A container extension.

**/
class CContainerComponent
	: public IEntityComponent
	, public IInteractionOpenable
	, public IInteractionLockable
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	// ~IEntityComponent

public:
	CContainerComponent() {}
	virtual ~CContainerComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CContainerComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{494F4D75-02A8-4A53-83E4-153D0D45CEEB}"_cry_guid;
		return id;
	}

	// IInteractionOpenable
	void OnInteractionOpenableOpen(IActorComponent& actor) override { CryLogAlways("OnInteractionOpenableOpen fired."); };
	void OnInteractionOpenableClose(IActorComponent& actor) override { CryLogAlways("OnInteractionOpenableClose fired."); };
	// ~IInteractionOpenable

	// IInteractionLockable
	void OnInteractionLockableLock(IActorComponent& actor) override { CryLogAlways("OnInteractionLockableLock fired."); };
	void OnInteractionLockableUnlock(IActorComponent& actor) override { CryLogAlways("OnInteractionLockableUnlock fired."); };
	// ~IInteractionLockable

	// Called on entity spawn, or when the state of the entity changes in Editor
	virtual void OnResetState();

private:
	/** Model for the geometry. */
	Cry::DefaultComponents::CStaticMeshComponent* m_pGeometryComponent { nullptr };

	/** Animation for the geometry. */
	CSimpleAnimationComponent* m_pSimpleAnimationComponent { nullptr };

	/** Doors should be lockable. */
	CLockableComponent* m_lockableComponent { nullptr };

	/** This entity should be interactive. */
	CEntityInteractionComponent* m_interactor { nullptr };
};
}