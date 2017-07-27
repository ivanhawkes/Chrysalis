#pragma once

#include "Helpers/DesignerEntityComponent.h"


/**
A Equipment extension.

\sa IEntityComponent.
\sa IEntityPropertyGroup.
\sa IInteractionEquipment.
\sa IInteractionLockable.
**/
class CEquipmentComponent : public CDesignerEntityComponent<>, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CEquipmentComponent, "Equipment", 0x9CDB533F110841B2, 0x83A9A7C757F74AB4)

public:
	// IEntityComponent
	void Initialize() override;
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	const char* GetLabel() const override { return "Equipment Properties"; };
	void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// CEquipmentComponent
	CEquipmentComponent() {};
	virtual ~CEquipmentComponent() {};

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;

	// Called on entity spawn, or when the state of the entity changes in Editor
	void OnResetState() override;

private:
};