#pragma once

#include "Helpers/DesignerEntityComponent.h"
#include "Snaplocks/Snaplock.h"

// Remove after testing is complete.
#include "Actor/Character/Character.h"


/**
A component that manages the snaplock system for an entity.

\sa IEntityComponent
\sa IEntityPropertyGroup
**/
class CSnaplockComponent : public CDesignerEntityComponent<>, public IEntityPropertyGroup
{
	CRY_ENTITY_COMPONENT_INTERFACE_AND_CLASS(CSnaplockComponent, "Snaplock", 0x70010D30B3704ACA, 0xB664623BDB7FA70C)

public:
	// IEntityComponent
	void Initialize() override;

	struct IEntityPropertyGroup* GetPropertyGroup() override { return this; }
	// ~IEntityComponent

	// IEntityPropertyGroup
	const char* GetLabel() const override { return "Snaplock Properties"; };
	void SerializeProperties(Serialization::IArchive& archive) override;
	// ~IEntityPropertyGroup

	// ***
	// *** CSnaplockComponent
	// ***

	CSnaplockComponent() {};
	virtual ~CSnaplockComponent() {};

	void OnResetState() override;

	void AddSnaplock(ISnaplock snaplock);

private:
	ISnaplock m_snaplock { SLT_ROOT, false };
	string m_definitionFile;
};