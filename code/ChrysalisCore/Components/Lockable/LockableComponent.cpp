#include <StdAfx.h>

#include "LockableComponent.h"


namespace Chrysalis
{
void CLockableComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CLockableComponent::ReflectType(Schematyc::CTypeDesc<CLockableComponent>& desc)
{
	desc.SetGUID(CLockableComponent::IID());
	desc.SetEditorCategory("Locks");
	desc.SetLabel("Lockable");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/door.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });

	desc.AddMember(&CLockableComponent::m_isLocked, 'lock', "IsLocked", "Is Locked?", "Is this locked?", true);
}


void CLockableComponent::OnResetState()
{
}
}