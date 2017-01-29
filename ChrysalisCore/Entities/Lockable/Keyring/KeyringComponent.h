#pragma once

#include "Helpers/DesignerEntityComponent.h"


struct IKeyringComponent : public IEntityComponent
{
	CRY_ENTITY_COMPONENT_INTERFACE(IKeyringComponent, 0x37D981514F55495F, 0x91C09099BCCA2300)
};


/**
A key extension.

\sa IEntityComponent
\sa IEntityPropertyGroup
**/
class CKeyringComponent final : public IKeyringComponent, public IEntityPropertyGroup
{
//	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CKeyringComponent, "KeyringComponent", 0xA0E8260A5B67C71, 0x814ECD1EAC0A85BC)
	CRY_ENTITY_COMPONENT_CLASS(CKeyringComponent, IKeyringComponent, "KeyringComponent", 0xA0E8260A5B67C71, 0x814ECD1EAC0A85BC);

public:
	// IEntityComponent
	void Initialize() override {};
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_START_LEVEL) | BIT64(ENTITY_EVENT_RESET) | BIT64(ENTITY_EVENT_EDITOR_PROPERTY_CHANGED) | BIT64(ENTITY_EVENT_XFORM_FINISHED_EDITOR); }
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	const char* GetLabel() const override { return "Keyring Properties"; };
	void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// ***
	// *** CKeyringComponent
	// ***

	CKeyringComponent() {};
	virtual ~CKeyringComponent() {};

	struct SExternalCVars
	{
		int m_debug;
	};
	const SExternalCVars &GetCVars() const;

private:
	string m_keys;
	bool m_bActive { true };

	void OnResetState();
};