#include <StdAfx.h>

#include "KeyringComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"


namespace Chrysalis
{
static void RegisterKeyringComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CKeyringComponent));
		// Functions
		{
		}
	}
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
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterKeyringComponent)
}