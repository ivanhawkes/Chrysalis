#include <StdAfx.h>

#include "KeyringComponent.h"
#include <CrySerialization/Decorators/Resources.h>


CRYREGISTER_CLASS(CKeyringComponent)


class CKeyringExtensionRegistrator : public IEntityRegistrator, public CKeyringComponent::SExternalCVars
{
	virtual void Register() override
	{
		// Register the entity class.
		RegisterEntityWithDefaultComponent<CKeyringComponent>("Keyring", "Locks", "door.bmp");

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


void CKeyringComponent::SerializeProperties(Serialization::IArchive& archive)
{
	archive(m_bActive, "Active", "Active");
	archive(m_keys, "Keys", "Keys");

	if (archive.isInput())
	{
		OnResetState();
	}
}


void CKeyringComponent::OnResetState()
{
}
