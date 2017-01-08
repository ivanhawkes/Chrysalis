#include <StdAfx.h>

#include "LockableComponent.h"
#include "Plugin/ChrysalisCorePlugin.h"


CRYREGISTER_CLASS(CLockableComponent)


class CLockableExtensionRegistrator : public IEntityRegistrator, public CLockableComponent::SExternalCVars
{
	virtual void Register() override
	{
		// Register the entity class.
		RegisterEntityWithDefaultComponent<CLockableComponent>("Lockable", "Locks", "Light.bmp");

		RegisterCVars();
	}

	void Unregister() override {};

	void RegisterCVars()
	{
		REGISTER_CVAR2("entity_lockable_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
	}
};

CLockableExtensionRegistrator g_LockableExtensionRegistrator;


const CLockableComponent::SExternalCVars& CLockableComponent::GetCVars() const
{
	return g_LockableExtensionRegistrator;
}


void CLockableComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		// Physicalize on level start for Launcher
		case ENTITY_EVENT_START_LEVEL:

			// Editor specific, physicalize on reset, property change or transform change
		case ENTITY_EVENT_RESET:
		case ENTITY_EVENT_EDITOR_PROPERTY_CHANGED:
		case ENTITY_EVENT_XFORM_FINISHED_EDITOR:
			Reset();
			break;
	}
}


void CLockableComponent::Reset()
{
}


void CLockableComponent::SerializeProperties(Serialization::IArchive& archive)
{
	archive(m_isLocked, "IsLocked", "IsLocked");

	if (!archive.isInput())
	{
		Reset();
	}
}
