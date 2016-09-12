#include <StdAfx.h>

#include "SwitchableComponent.h"
#include <Game/GameFactory.h>


class CSwitchableComponentRegistrator
	: public IEntityRegistrator
	, public CSwitchableComponent::SExternalCVars
{
	virtual void Register() override
	{
		CSwitchableComponent::Register();
		RegisterCVars();
	}

	void RegisterCVars()
	{
		REGISTER_CVAR2("entity_switchable_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
	}
};

CSwitchableComponentRegistrator g_SwitchableComponentRegistrator;

const CSwitchableComponent::SExternalCVars& CSwitchableComponent::GetCVars() const
{
	return g_SwitchableComponentRegistrator;
}


// ***
// *** IGameObjectExtension
// ***


void CSwitchableComponent::PostInit(IGameObject * pGameObject)
{
}


void CSwitchableComponent::Update(SEntityUpdateContext& ctx, int updateSlot)
{
}


// ***
// *** CSwitchableComponent
// ***


void CSwitchableComponent::Register()
{
	auto properties = new SNativeEntityPropertyInfo [eNumProperties];
	memset(properties, 0, sizeof(SNativeEntityPropertyInfo) * eNumProperties);

	{	// Switchable
		ENTITY_PROPERTY_GROUP("Switchable", ePropertyGroup_SwitchableBegin, ePropertyGroup_SwitchableEnd, properties);
		RegisterEntityProperty<bool>(properties, eProperty_Switchable_Enabled, "Enabled", "1", "Is the switch enabled?", 0.0f, 1.0f);
		RegisterEntityProperty<bool>(properties, eProperty_Switchable_IsSwitchOn, "IsSwitchOn", "1", "Is this entity switched on?", 0.0f, 1.0f);
	}	// ~Switchable

	// Finally, register the entity class so that instances can be created later on either via Launcher or Editor
	CGameFactory::RegisterNativeEntity<CSwitchableComponent>("Switchable", "Switches", "Light.bmp", CGameFactory::eGameObjectRegistrationFlags::eGORF_HiddenInEditor, properties, eNumProperties);
}
