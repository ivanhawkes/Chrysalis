#pragma once

#include "Snaplocks/Snaplock.h"

namespace Chrysalis
{
/** A component that manages the snaplock system for an entity. */
class CSnaplockComponent
	: public IEntityComponent
{
protected:
	friend CChrysalisCorePlugin;
	static void Register(Schematyc::CEnvRegistrationScope& componentScope);

	// IEntityComponent
	void Initialize() override;
	// ~IEntityComponent

public:
	CSnaplockComponent() {}
	virtual ~CSnaplockComponent() {}

	static void ReflectType(Schematyc::CTypeDesc<CSnaplockComponent>& desc);

	static CryGUID& IID()
	{
		static CryGUID id = "{A3C41456-80F1-4E0D-9B46-62685F2AEE4A}"_cry_guid;
		return id;
	}

	virtual void OnResetState();

	void AddSnaplock(ISnaplock snaplock);

private:
	ISnaplock m_snaplock { SLT_ROOT, false };
	string m_definitionFile;
};
}