#include <StdAfx.h>

#include "KeyringComponent.h"

namespace Chrysalis
{
void CKeyringComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CKeyringComponent::ReflectType(Schematyc::CTypeDesc<CKeyringComponent>& desc)
{
	desc.SetGUID(CKeyringComponent::IID());
	desc.SetEditorCategory("Locks");
	desc.SetLabel("Keyring");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });

	desc.AddMember(&CKeyringComponent::m_bActive, 'actv', "Active", "Active", "Is this active?", true);
	// TODO: CRITICAL: HACK: BROKEN: !!
	//	desc.AddMember(&CKeyringComponent::m_keys, 'keys', "Keys", "Keys", "Keys on this keyring.", "");
}


void CKeyringComponent::OnResetState()
{
}
}