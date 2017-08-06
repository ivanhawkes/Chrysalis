#include <StdAfx.h>

#include "InventoryComponent.h"


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
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });
}


void CInventoryComponent::Initialize()
{
	const auto pEntity = GetEntity();

	OnResetState();
}


void CInventoryComponent::OnResetState()
{
}
}