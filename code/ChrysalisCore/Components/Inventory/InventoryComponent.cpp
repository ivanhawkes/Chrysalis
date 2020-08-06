#include <StdAfx.h>

#include "InventoryComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"
#include <Components/Actor/ActorComponent.h>


namespace Chrysalis
{
static void RegisterInventoryComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CInventoryComponent));
		// Functions
		{
		}
	}
}


void CInventoryComponent::ReflectType(Schematyc::CTypeDesc<CInventoryComponent>& desc)
{
	desc.SetGUID(CInventoryComponent::IID());
	desc.SetEditorCategory("Inventory");
	desc.SetLabel("Inventory");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({IEntityComponent::EFlags::Singleton});

	// Mark the actor component as a hard requirement.
	desc.AddComponentInteraction(SEntityComponentRequirements::EType::HardDependency, CActorComponent::IID());
}


void CInventoryComponent::Initialize()
{
	OnResetState();
}


void CInventoryComponent::OnResetState()
{
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterInventoryComponent)
}