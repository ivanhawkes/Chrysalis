#include <StdAfx.h>

#include "EquipmentComponent.h"

CRYREGISTER_CLASS(CEquipmentComponent)


class CEquipmentExtensionRegistrator : public IEntityRegistrator, public CEquipmentComponent::SExternalCVars
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CEquipmentComponent>("Equipment", "Equipment", "door.bmp");

		RegisterCVars();
	}

	void RegisterCVars()
	{
		REGISTER_CVAR2("entity_Equipment_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
	}
};

CEquipmentExtensionRegistrator g_EquipmentExtensionRegistrator;

const CEquipmentComponent::SExternalCVars& CEquipmentComponent::GetCVars() const
{
	return g_EquipmentExtensionRegistrator;
}


void CEquipmentComponent::Initialize()
{
	auto pEntity = GetEntity();

	OnResetState();
}


void CEquipmentComponent::OnResetState()
{
}


void CEquipmentComponent::SerializeProperties(Serialization::IArchive& archive)
{
	if (archive.isInput())
	{
		OnResetState();
	}
}
