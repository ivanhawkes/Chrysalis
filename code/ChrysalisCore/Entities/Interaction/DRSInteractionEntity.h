#pragma once

#include "Helpers/DesignerEntityComponent.h"


class CDRSInteractionEntity final : public CDesignerEntityComponent<>, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CDRSInteractionEntity, "DRSInteractionEntity", 0xA723315D3E5345E3, 0x90766D3684FB132F);

	virtual ~CDRSInteractionEntity() {}

public:
	// IEntityComponent
	void Initialize() override;
	virtual IEntityPropertyGroup* GetPropertyGroup() final { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	virtual const char* GetLabel() const override { return "DRS Entity Properties"; }
	virtual void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// IDRSInteractionEntityComponent
	virtual void OnResetState() final;
	// IDRSInteractionEntityComponent
};
