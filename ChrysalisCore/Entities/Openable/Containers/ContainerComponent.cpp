#include <StdAfx.h>

#include "ContainerComponent.h"


CRYREGISTER_CLASS(CContainerComponent)


class CContainerExtensionRegistrator : public IEntityRegistrator, public CContainerComponent::SExternalCVars
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CContainerComponent>("Container", "Containers", "Light.bmp");

		RegisterCVars();
	}

	void RegisterCVars()
	{
		REGISTER_CVAR2("entity_containerextension_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
	}
};

CContainerExtensionRegistrator g_ContainerExtensionRegistrator;

const CContainerComponent::SExternalCVars& CContainerComponent::GetCVars() const
{
	return g_ContainerExtensionRegistrator;
}


void CContainerComponent::ProcessEvent(SEntityEvent& event)
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


void CContainerComponent::OnResetState()
{
}



void CContainerComponent::SerializeProperties(Serialization::IArchive& archive)
{
	if (archive.isInput())
	{
		OnResetState();
	}
}
