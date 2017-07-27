#include <StdAfx.h>

#include "CharacterAttributesComponent.h"

CRYREGISTER_CLASS(CCharacterAttributesComponent)


class CCharacterAttributesExtensionRegistrator : public IEntityRegistrator, public CCharacterAttributesComponent::SExternalCVars
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CCharacterAttributesComponent>("CharacterAttributes", "Character Attributes", "door.bmp");

		RegisterCVars();
	}

	void RegisterCVars()
	{
		REGISTER_CVAR2("entity_CharacterAttributes_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
	}
};

CCharacterAttributesExtensionRegistrator g_CharacterAttributesExtensionRegistrator;

const CCharacterAttributesComponent::SExternalCVars& CCharacterAttributesComponent::GetCVars() const
{
	return g_CharacterAttributesExtensionRegistrator;
}


void CCharacterAttributesComponent::Initialize()
{
	auto pEntity = GetEntity();

	OnResetState();
}


void CCharacterAttributesComponent::OnResetState()
{
}


void CCharacterAttributesComponent::SerializeProperties(Serialization::IArchive& archive)
{
	if (archive.isInput())
	{
		OnResetState();
	}
}
