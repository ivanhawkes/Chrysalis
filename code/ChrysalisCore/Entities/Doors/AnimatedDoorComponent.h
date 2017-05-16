#pragma once

#include "Helpers/DesignerEntityComponent.h"
#include <Entities/Interaction/IEntityInteraction.h>


class CGeometryComponent;
class CLockableComponent;
class CEntityInteractionComponent;
class CControlledAnimationComponent;
class CSimpleAnimationComponent;


/**
An animated door.

\sa IEntityComponent
\sa IEntityPropertyGroup
\sa IInteractionContainer
\sa IInteractionOpenable
\sa IInteractionLockable
**/
class CAnimatedDoorComponent : public CDesignerEntityComponent<>, public IEntityPropertyGroup, public IInteractionInteract, public IInteractionOpenable, public IInteractionLockable
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CAnimatedDoorComponent, "AnimatedDoor", 0xD246E11FE7E248F0, 0xB512402908F84496)

public:
	// IEntityComponent
	void Initialize() override;
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IInteractionInteract
	void OnInteractionInteract() override;
	// ~IInteractionInteract

	// IEntityPropertyGroup
	const char* GetLabel() const override { return "AnimatedDoor Properties"; };
	void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// IInteractionOpenable
	void OnInteractionOpenableOpen() override { gEnv->pLog->LogAlways("OnInteractionOpenableOpen fired."); };
	void OnInteractionOpenableClose() override { gEnv->pLog->LogAlways("OnInteractionOpenableClose fired."); };
	// ~IInteractionOpenable

	// IInteractionLockable
	void OnInteractionLockableLock() override { gEnv->pLog->LogAlways("OnInteractionLockableLock fired."); };
	void OnInteractionLockableUnlock() override { gEnv->pLog->LogAlways("OnInteractionLockableUnlock fired."); };
	// ~IInteractionLockable

	// IInteractionLockable
	CAnimatedDoorComponent() {};
	virtual ~CAnimatedDoorComponent() {}

private:
	const string kDoorAnimationOpen { "default" };
	const string kDoorAnimationClose { "down" };

	// Called on entity spawn, or when the state of the entity changes in Editor
	void OnResetState() override;

	/** Model for the geometry. */
	CGeometryComponent* m_pGeometryComponent { nullptr };

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