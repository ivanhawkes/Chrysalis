#include <StdAfx.h>

#include "CharacterAttributesComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"


namespace Chrysalis
{
static void RegisterCharacterAttributesComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CCharacterAttributesComponent));
		// Functions
		{
		}
	}
}


void CCharacterAttributesComponent::ReflectType(Schematyc::CTypeDesc<CCharacterAttributesComponent>& desc)
{
	desc.SetGUID(CCharacterAttributesComponent::IID());
	desc.SetEditorCategory("Character Attributes");
	desc.SetLabel("Character Attributes");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({IEntityComponent::EFlags::Singleton});
}


void CCharacterAttributesComponent::Initialize()
{
	OnResetState();
}


void CCharacterAttributesComponent::OnResetState()
{
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterCharacterAttributesComponent)
}