#include <StdAfx.h>

#include "SnaplockComponent.h"


CRYREGISTER_CLASS(CSnaplockComponent)


class CSnaplockExtensionRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		// Register the entity class.
		RegisterEntityWithDefaultComponent<CSnaplockComponent>("Snaplock", "Snaplocks", "door.bmp");
	}
};

CSnaplockExtensionRegistrator g_SnaplockExtensionRegistrator;


void CSnaplockComponent::Initialize()
{
	OnResetState();
}


void CSnaplockComponent::SerializeProperties(Serialization::IArchive& archive)
{
	archive(m_snaplock, "Snaplocks", "Snaplocks");
	archive(m_definitionFile, "definitionFile", "Definition File");
	
	if (archive.isInput())
	{
		OnResetState();
	}
}


void CSnaplockComponent::OnResetState()
{
}


void CSnaplockComponent::AddSnaplock(ISnaplock snaplock)
{
	m_snaplock.AddSnaplock(snaplock);
}
