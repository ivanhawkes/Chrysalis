#include <StdAfx.h>

#include "SnaplockComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"


namespace Chrysalis
{
static void RegisterSnaplockComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CSnaplockComponent));
		// Functions
		{
		}
	}
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

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterSnaplockComponent)
}