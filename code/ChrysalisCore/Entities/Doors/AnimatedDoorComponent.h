#pragma once

#include <Entities/Interaction/IEntityInteraction.h>
#include <DefaultComponents/Geometry/AnimatedMeshComponent.h>


class Cry::DefaultComponents::CAnimatedMeshComponent;


namespace Chrysalis
{
class CLockableComponent;
class CEntityInteractionComponent;
class CControlledAnimationComponent;
class CSimpleAnimationComponent;


/**
An animated door.

**/
class CAnimatedDoorComponent
	: public IEntityComponent
	, public IInteractionInteract, public IInteractionOpenable, public IInteractionLockable
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	// ~IEntityComponent

public:
	CAnimatedDoorComponent() {}
	virtual ~CAnimatedDoorComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CAnimatedDoorComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{749AE78C-8577-4EF6-86E0-5B1E4C8ADF80}"_cry_guid;
		return id;
	}

	// IInteractionInteract
	void OnInteractionInteract() override;
	// ~IInteractionInteract

	// IInteractionOpenable
	void OnInteractionOpenableOpen() override { gEnv->pLog->LogAlways("OnInteractionOpenableOpen fired."); };
	void OnInteractionOpenableClose() override { gEnv->pLog->LogAlways("OnInteractionOpenableClose fired."); };
	// ~IInteractionOpenable

	// IInteractionLockable
	void OnInteractionLockableLock() override { gEnv->pLog->LogAlways("OnInteractionLockableLock fired."); };
	void OnInteractionLockableUnlock() override { gEnv->pLog->LogAlways("OnInteractionLockableUnlock fired."); };
	// ~IInteractionLockable

private:
	const Schematyc::LowLevelAnimationName kDoorAnimationOpen { "default" };
	const Schematyc::LowLevelAnimationName kDoorAnimationClose { "down" };

	// Called on entity spawn, or when the state of the entity changes in Editor
	virtual void OnResetState();

	/** Model for the geometry. */
	Cry::DefaultComponents::CAnimatedMeshComponent* m_pGeometryComponent { nullptr };

	/** Animation for the geometry. */
	//CControlledAnimationComponent* m_pAnimationComponent { nullptr };
	CSimpleAnimationComponent* m_pAnimationComponent { nullptr };

	/** Doors should be lockable. */
	CLockableComponent* m_pLockableComponent { nullptr };

	/** This entity should be interactive. */
	CEntityInteractionComponent* m_interactor { nullptr };

	/** Is the door open? */
	bool m_IsOpen { false };
};
}