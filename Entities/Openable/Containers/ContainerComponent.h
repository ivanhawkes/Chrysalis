#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntitySystem.h>
#include "Entities/Interaction/IEntityInteraction.h"


/**
A container extension.

\sa IEntityComponent
\sa IEntityPropertyGroup
\sa IInteractionContainer
**/
class CContainerComponent : public IEntityComponent, public IEntityPropertyGroup, public IInteractionContainer
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CContainerComponent, "Container", 0x6FE7D7D95B364222, 0xB235D9C3207C8956)

public:
	// IEntityComponent
	void Initialize() override {};
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_START_LEVEL) | BIT64(ENTITY_EVENT_RESET) | BIT64(ENTITY_EVENT_EDITOR_PROPERTY_CHANGED) | BIT64(ENTITY_EVENT_XFORM_FINISHED_EDITOR); }
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	const char* GetLabel() const { return "Container"; };
	void SerializeProperties(Serialization::IArchive& archive);
	// ~IEntityPropertyGroup

	// IInteractionContainer
	void ContainerOpen() override {};
	void ContainerClose() override {};
	void ContainerLock() override {};
	void ContainerUnlock() override {};
	// ~IInteractionContainer

	// CContainerComponent
	CContainerComponent() {};
	virtual ~CContainerComponent() {};

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;

private:
	// Called on entity spawn, or when the state of the entity changes in Editor
	void Reset();
};