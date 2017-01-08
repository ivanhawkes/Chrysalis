#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntitySystem.h>


/**
A key extension.

\sa IEntityComponent
\sa IEntityPropertyGroup
**/
class CKeyringComponent : public IEntityComponent, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CKeyringComponent, "Keyring", 0xA0E8260A5B67C71, 0x814ECD1EAC0A85BC)

public:
	// IEntityComponent
	void Initialize() override {};
	void ProcessEvent(SEntityEvent& event) override;
	uint64 GetEventMask() const { return BIT64(ENTITY_EVENT_START_LEVEL) | BIT64(ENTITY_EVENT_RESET) | BIT64(ENTITY_EVENT_EDITOR_PROPERTY_CHANGED) | BIT64(ENTITY_EVENT_XFORM_FINISHED_EDITOR); }
	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	const char* GetLabel() const { return "Keyring"; };
	void SerializeProperties(Serialization::IArchive& archive);
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

	void Reset();
};