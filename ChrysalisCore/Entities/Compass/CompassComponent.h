#pragma once

#include "Helpers/DesignerEntityComponent.h"
#include <Components/Interaction/EntityInteractionComponent.h>
#include <Components/Geometry/GeometryComponent.h>
#include <Components/Animation/ControlledAnimationComponent.h>


class CCompassComponent final : public CDesignerEntityComponent<>, public IEntityPropertyGroup, public IInteractionItem
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CCompassComponent, "CompassComponent", 0x7C1B29DCC22B4F28, 0x9A38858A4566929E)
	virtual ~CCompassComponent() {}

public:
	// IEntityComponent
	virtual void Initialize() final;
	virtual IEntityPropertyGroup* GetPropertyGroup() final { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "Compass Properties"; }
	virtual void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// ICompassEntityComponent
	virtual void OnResetState() final;
	// ICompassEntityComponent

	// IInteractionItem
	void OnInteractionItemInspect() override { gEnv->pLog->LogAlways("OnInteractionItemInspect fired."); };
	void OnInteractionItemPickup() override { gEnv->pLog->LogAlways("OnInteractionItemPickup fired."); };
	void OnInteractionItemDrop() override { gEnv->pLog->LogAlways("OnInteractionItemDrop fired."); };
	void OnInteractionItemToss() override { gEnv->pLog->LogAlways("OnInteractionItemToss fired."); };
	// IInteractionItem

private:
	/** Model for the geometry. */
	CGeometryComponent* m_pGeometryComponent { nullptr };

	/** Animation for the geometry. */
	CControlledAnimationComponent* m_pControlledAnimationComponent { nullptr };

	/** This entity should be interactive. */
	CEntityInteractionComponent* m_interactor { nullptr };
};
