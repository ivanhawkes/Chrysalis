#include <StdAfx.h>

#include "EquipmentComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"


namespace Chrysalis
{
static void RegisterEquipmentComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CEquipmentComponent));
		// Functions
		{
		}
	}
}


void CEquipmentComponent::ReflectType(Schematyc::CTypeDesc<CEquipmentComponent>& desc)
{
	desc.SetGUID(CEquipmentComponent::IID());
	desc.SetEditorCategory("Equipment");
	desc.SetLabel("Equipment");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({IEntityComponent::EFlags::Singleton});
}


void CEquipmentComponent::Initialize()
{
	OnResetState();
}


void CEquipmentComponent::OnResetState()
{
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterEquipmentComponent)
}