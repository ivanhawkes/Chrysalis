#pragma once

#include "Helpers/DesignerEntityComponent.h"
#include "Entities/Interaction/IEntityInteraction.h"

class CGeometryComponent;
class CSimpleAnimationComponent;
class CLockableComponent;
class CEntityInteractionComponent;

/**
A container extension.

\sa IEntityComponent
\sa IEntityPropertyGroup
\sa IInteractionContainer
\sa IInteractionLockable
**/
class CContainerComponent : public CDesignerEntityComponent<>, public IEntityPropertyGroup, public IInteractionOpenable, public IInteractionLockable
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CContainerComponent, "Container", 0x6FE7D7D95B364222, 0xB235D9C3207C8956)

public:
	// IEntityComponent
	void Initialize() override;
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	const char* GetLabel() const override { return "Container Properties"; };
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

	// CContainerComponent
	CContainerComponent() {};
	virtual ~CContainerComponent() {};

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;

	// Called on entity spawn, or when the state of the entity changes in Editor
	void OnResetState() override;

private:
	/** Model for the geometry. */
	CGeometryComponent* m_pGeometryComponent { nullptr };

	/** Animation for the geometry. */
	CSimpleAnimationComponent* m_pSimpleAnimationComponent { nullptr };

	/** Doors should be lockable. */
	CLockableComponent* m_lockableComponent { nullptr };

	/** This entity should be interactive. */
	CEntityInteractionComponent* m_interactor { nullptr };
};