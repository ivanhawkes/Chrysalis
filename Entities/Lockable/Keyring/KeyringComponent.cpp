#include <StdAfx.h>

#include "KeyringComponent.h"


class CKeyringExtensionRegistrator
	: public IEntityRegistrator
	, public CKeyringComponent::SExternalCVars
{
	virtual void Register() override
	{
		CKeyringComponent::Register();
		RegisterCVars();
	}

	void RegisterCVars()
	{
		REGISTER_CVAR2("entity_keyringextension_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
	}
};

CKeyringExtensionRegistrator g_KeyringExtensionRegistrator;

const CKeyringComponent::SExternalCVars& CKeyringComponent::GetCVars() const
{
	return g_KeyringExtensionRegistrator;
}


// ***
// *** IGameObjectExtension
// *** 


void CKeyringComponent::PostInit(IGameObject * pGameObject)
{
}


void CKeyringComponent::Update(SEntityUpdateContext& ctx, int updateSlot)
{
}


// ***
// *** CKeyringComponent
// ***


void CKeyringComponent::Register()
{
	auto properties = new SNativeEntityPropertyInfo [eNumProperties];
	memset(properties, 0, sizeof(SNativeEntityPropertyInfo) * eNumProperties);

	{	// Keyring
		ENTITY_PROPERTY_GROUP("Keyring", ePropertyGroup_KeyringBegin, ePropertyGroup_KeyringEnd, properties);
		RegisterEntityProperty<string>(properties, eProperty_Keyring_Keys, "Keys", "", "Comma separated list of key Ids.", 0.0f, 1.0f);
	}	// ~Keyring

	// Finally, register the entity class so that instances can be created later on either via Launcher or Editor
	CGameFactory::RegisterNativeEntity<CKeyringComponent>("Keyring", "Keyring", "Light.bmp", CGameFactory::eGameObjectRegistrationFlags::eGORF_HiddenInEditor, properties, eNumProperties);
}
