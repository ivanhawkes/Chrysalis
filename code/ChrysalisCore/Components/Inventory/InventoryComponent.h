#pragma once

#include "Helpers/DesignerEntityComponent.h"


/**
A Inventory extension.

\sa IEntityComponent.
\sa IEntityPropertyGroup.
\sa IInteractionInventory.
\sa IInteractionLockable.
**/
class CInventoryComponent : public CDesignerEntityComponent<>, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CInventoryComponent, "Inventory", 0x0E58AE4CF0B84AE8, 0x8BB0C7CC5F585495)

public:
	// IEntityComponent
	void Initialize() override;
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	const char* GetLabel() const override { return "Inventory Properties"; };
	void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// CInventoryComponent
	CInventoryComponent() {};
	virtual ~CInventoryComponent() {};

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;

	// Called on entity spawn, or when the state of the entity changes in Editor
	void OnResetState() override;

private:
};