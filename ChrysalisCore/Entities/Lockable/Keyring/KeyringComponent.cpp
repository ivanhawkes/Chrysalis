#include <StdAfx.h>

#include "KeyringComponent.h"
#include <CrySerialization/Decorators/Resources.h>


CRYREGISTER_CLASS(CKeyringComponent)


class CKeyringExtensionRegistrator : public IEntityRegistrator, public CKeyringComponent::SExternalCVars
{
	virtual void Register() override
	{
		// Register the entity class.
		RegisterEntityWithDefaultComponent<CKeyringComponent>("Keyring", "Locks", "Light.bmp");

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


void CKeyringComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		// Physicalize on level start for Launcher
		case ENTITY_EVENT_START_LEVEL:

			// Editor specific, physicalize on reset, property change or transform change
		case ENTITY_EVENT_RESET:
		case ENTITY_EVENT_EDITOR_PROPERTY_CHANGED:
		case ENTITY_EVENT_XFORM_FINISHED_EDITOR:
			OnResetState();
			break;
	}
}


void CKeyringComponent::OnResetState()
{
	int a = 1;
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
