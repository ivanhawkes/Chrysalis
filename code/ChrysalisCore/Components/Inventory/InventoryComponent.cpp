#include <StdAfx.h>

#include "InventoryComponent.h"
#include <Actor/ActorComponent.h>


namespace Chrysalis
{
void CInventoryComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CInventoryComponent::ReflectType(Schematyc::CTypeDesc<CInventoryComponent>& desc)
{
	desc.SetGUID(CInventoryComponent::IID());
	desc.SetEditorCategory("Inventory");
	desc.SetLabel("Inventory");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });

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
}