#include <StdAfx.h>

#include "SnaplockComponent.h"


namespace Chrysalis
{
void CSnaplockComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CSnaplockComponent::ReflectType(Schematyc::CTypeDesc<CSnaplockComponent>& desc)
{
	desc.SetGUID(CSnaplockComponent::IID());
	desc.SetEditorCategory("Snaplocks");
	desc.SetLabel("Snaplocks");
	desc.SetDescription("Deprecate this?");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });
}


void CSnaplockComponent::Initialize()
{
	OnResetState();
}


void CSnaplockComponent::OnResetState()
{
}


void CSnaplockComponent::AddSnaplock(ISnaplock snaplock)
{
	m_snaplock.AddSnaplock(snaplock);
}
}