#pragma once

#include "Helpers/DesignerEntityComponent.h"


/**
A CharacterAttributes extension.

\sa IEntityComponent.
\sa IEntityPropertyGroup.
\sa IInteractionCharacterAttributes.
\sa IInteractionLockable.
**/
class CCharacterAttributesComponent : public CDesignerEntityComponent<>, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CCharacterAttributesComponent, "CharacterAttributes", 0x208A5A90F5EC4C86, 0xA61E5695005E2E73)

public:
	// IEntityComponent
	void Initialize() override;
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	const char* GetLabel() const override { return "Character Attributes Properties"; };
	void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// CCharacterAttributesComponent
	CCharacterAttributesComponent() {};
	virtual ~CCharacterAttributesComponent() {};

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;

	// Called on entity spawn, or when the state of the entity changes in Editor
	void OnResetState() override;

private:
};