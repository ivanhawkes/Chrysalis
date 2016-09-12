#include <StdAfx.h>

#include "ContainerComponent.h"


class CContainerExtensionRegistrator
	: public IEntityRegistrator
	, public CContainerComponent::SExternalCVars
{
	virtual void Register() override
	{
		CGameFactory::RegisterGameObjectExtension<CContainerComponent>("Container");

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


// ***
// *** IGameObjectExtension
// ***


void CContainerComponent::PostInit(IGameObject * pGameObject)
{
}


void CContainerComponent::Update(SEntityUpdateContext& ctx, int updateSlot)
{
}


// ***
// *** CContainerComponent
// ***
