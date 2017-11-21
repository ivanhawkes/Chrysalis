#include <StdAfx.h>

#include "CharacterAttributesComponent.h"


namespace Chrysalis
{
void CCharacterAttributesComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CCharacterAttributesComponent::ReflectType(Schematyc::CTypeDesc<CCharacterAttributesComponent>& desc)
{
	desc.SetGUID(CCharacterAttributesComponent::IID());
	desc.SetEditorCategory("Character Attributes");
	desc.SetLabel("Character Attributes");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
}


void CCharacterAttributesComponent::Initialize()
{
	OnResetState();
}


void CCharacterAttributesComponent::OnResetState()
{
}
}