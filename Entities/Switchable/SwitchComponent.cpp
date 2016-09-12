#include <StdAfx.h>

#include "SwitchComponent.h"
#include <Game/GameFactory.h>


class CSwitchComponentRegistrator
	: public IEntityRegistrator
	, public CSwitchComponent::SExternalCVars
{
	virtual void Register() override
	{
		CSwitchComponent::Register();
		RegisterCVars();
	}

	void RegisterCVars()
	{
		REGISTER_CVAR2("entity_switch_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
	}
};

CSwitchComponentRegistrator g_SwitchComponentRegistrator;

const CSwitchComponent::SExternalCVars& CSwitchComponent::GetCVars() const
{
	return g_SwitchComponentRegistrator;
}


// ***
// *** IGameObjectExtension
// ***


void CSwitchComponent::PostInit(IGameObject * pGameObject)
{
}


void CSwitchComponent::Update(SEntityUpdateContext& ctx, int updateSlot)
{
}


// ***
// *** CSwitchComponent
// ***


void CSwitchComponent::Register()
{
	auto properties = new SNativeEntityPropertyInfo [eNumProperties];
	memset(properties, 0, sizeof(SNativeEntityPropertyInfo) * eNumProperties);

	{	// Switch
		ENTITY_PROPERTY_GROUP("Switch", ePropertyGroup_SwitchBegin, ePropertyGroup_SwitchEnd, properties);
		RegisterEntityProperty<bool>(properties, eProperty_Switch_Enabled, "Enabled", "1", "Is the switch enabled?", 0.0f, 1.0f);
		RegisterEntityProperty<bool>(properties, eProperty_Switch_IsSwitchOn, "IsSwitchOn", "1", "Is the switch in the on position?", 0.0f, 1.0f);
	}	// ~Switch

	// Finally, register the entity class so that instances can be created later on either via Launcher or Editor
	CGameFactory::RegisterNativeEntity<CSwitchComponent>("SwitchComponent", "Switches", "Light.bmp", CGameFactory::eGameObjectRegistrationFlags::eGORF_HiddenInEditor, properties, eNumProperties);
}
