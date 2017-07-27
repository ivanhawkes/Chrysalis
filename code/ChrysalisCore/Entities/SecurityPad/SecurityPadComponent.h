#pragma once

#include "Helpers/DesignerEntityComponent.h"
#include <Components/Interaction/EntityInteractionComponent.h>
#include <Components/Geometry/GeometryComponent.h>


class CSecurityPadComponent final : public CDesignerEntityComponent<>, public IEntityPropertyGroup, public IInteractionExamine
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CSecurityPadComponent, "SecurityPadComponent", 0x0792FCE0BF7E44FC, 0xA06AA6F9B78EADC4)
	virtual ~CSecurityPadComponent() {}

public:
	// IEntityComponent
	virtual void Initialize() final;
	virtual IEntityPropertyGroup* GetPropertyGroup() final { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "SecurityPad Properties"; }
	virtual void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// ISecurityPadEntityComponent
	virtual void OnResetState() final;
	// ISecurityPadEntityComponent

	// IInteractionExamine
	void OnInteractionExamineStart() override;;
	void OnInteractionExamineComplete() override;;
	void OnInteractionExamineCancel() override;;
	// IInteractionExamine

private:
	/** Model for the geometry. */
	CGeometryComponent* m_pGeometryComponent { nullptr };

	/** This entity should be interactive. */
	CEntityInteractionComponent* m_interactor { nullptr };
};
