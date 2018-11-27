#include <StdAfx.h>

#include "EquipmentComponent.h"


namespace Chrysalis
{
void CEquipmentComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CEquipmentComponent::ReflectType(Schematyc::CTypeDesc<CEquipmentComponent>& desc)
{
	desc.SetGUID(CEquipmentComponent::IID());
	desc.SetEditorCategory("Equipment");
	desc.SetLabel("Equipment");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
}


void CEquipmentComponent::Initialize()
{
	OnResetState();
}


void CEquipmentComponent::OnResetState()
{
}
}