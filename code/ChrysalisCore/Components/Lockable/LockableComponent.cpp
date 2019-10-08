#include <StdAfx.h>

#include "LockableComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"


namespace Chrysalis
{
static void RegisterLockableComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CLockableComponent));
		// Functions
		{
		}
	}
}


void CLockableComponent::ReflectType(Schematyc::CTypeDesc<CLockableComponent>& desc)
{
	desc.SetGUID(CLockableComponent::IID());
	desc.SetEditorCategory("Locks");
	desc.SetLabel("Lockable");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });

	desc.AddMember(&CLockableComponent::m_isLocked, 'lock', "IsLocked", "Is Locked?", "Is this locked?", true);
}


void CLockableComponent::OnResetState()
{
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterLockableComponent)
}