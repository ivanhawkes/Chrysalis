#include <StdAfx.h>

#include "InventoryComponent.h"

CRYREGISTER_CLASS(CInventoryComponent)


class CInventoryExtensionRegistrator : public IEntityRegistrator, public CInventoryComponent::SExternalCVars
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CInventoryComponent>("Inventory", "Inventory", "door.bmp");

		RegisterCVars();
	}

	void RegisterCVars()
	{
		REGISTER_CVAR2("entity_Inventory_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
	}
};

CInventoryExtensionRegistrator g_InventoryExtensionRegistrator;

const CInventoryComponent::SExternalCVars& CInventoryComponent::GetCVars() const
{
	return g_InventoryExtensionRegistrator;
}


void CInventoryComponent::Initialize()
{
	auto pEntity = GetEntity();

	OnResetState();
}


void CInventoryComponent::OnResetState()
{
}


void CInventoryComponent::SerializeProperties(Serialization::IArchive& archive)
{
	if (archive.isInput())
	{
		OnResetState();
	}
}
