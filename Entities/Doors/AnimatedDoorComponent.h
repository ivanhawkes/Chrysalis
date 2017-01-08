#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntitySystem.h>
#include <Entities/Interaction/EntityInteractionComponent.h>


class CLockableComponent;


/**
An animated door.

\sa IEntityComponent
\sa IEntityPropertyGroup
\sa IInteractionContainer
**/
class CAnimatedDoorComponent : public IEntityComponent, public IEntityPropertyGroup, public IInteractionContainer
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CAnimatedDoorComponent, "AnimatedDoor", 0xD246E11FE7E248F0, 0xB512402908F84496)

public:
	// IEntityComponent
	void Initialize() override;
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_START_LEVEL) | BIT64(ENTITY_EVENT_RESET) | BIT64(ENTITY_EVENT_EDITOR_PROPERTY_CHANGED) | BIT64(ENTITY_EVENT_XFORM_FINISHED_EDITOR); }
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent
	
	// IEntityPropertyGroup
	const char* GetLabel() const { return "AnimatedDoor"; };
	void SerializeProperties(Serialization::IArchive& archive);
	// ~IEntityPropertyGroup

	// IInteractionContainer
	void ContainerOpen() override { gEnv->pLog->LogAlways("Interation Toggle fired."); };
	void ContainerClose() override { gEnv->pLog->LogAlways("Interation Toggle fired."); };
	void ContainerLock() override { gEnv->pLog->LogAlways("Interation Toggle fired."); };
	void ContainerUnlock() override { gEnv->pLog->LogAlways("Interation Toggle fired."); };
	// ~IInteractionContainer

	// CAnimatedDoorComponent
	CAnimatedDoorComponent() {};
	virtual ~CAnimatedDoorComponent() {}

private:
	// Called on entity spawn, or when the state of the entity changes in Editor
	void Reset();

	string m_geometry { "objects/default/primitive_box.cgf" };
	float m_mass { 1.0f };

	CLockableComponent* m_lockableExtension { nullptr };

	CEntityInteractionComponent* m_interactor { nullptr };
};