#include <StdAfx.h>

#include "LockableComponent.h"


CRYREGISTER_CLASS(CLockableComponent)


class CLockableExtensionRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		// Register the entity class.
		RegisterEntityWithDefaultComponent<CLockableComponent>("Lockable", "Locks", "door.bmp");
	}
};

CLockableExtensionRegistrator g_LockableExtensionRegistrator;


void CLockableComponent::SerializeProperties(Serialization::IArchive& archive)
{
	archive(m_isLocked, "IsLocked", "IsLocked");

	if (archive.isInput())
	{
		OnResetState();
	}
}


void CLockableComponent::OnResetState()
{
}
