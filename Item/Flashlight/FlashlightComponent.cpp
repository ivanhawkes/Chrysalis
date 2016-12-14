#include <StdAfx.h>

#include "FlashlightComponent.h"
#include <Game/GameFactory.h>
#include <GameXmlParamReader.h>
#include <Entities/Interaction/EntityInteractionComponent.h>


class CFlashlightRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		CFlashlightComponent::Register();
	}
};

CFlashlightRegistrator g_flashlightRegistrator;



// ***
// *** ISimpleItem
// ***


bool CFlashlightComponent::Init(IGameObject * pGameObject)
{
	return CItem::Init(pGameObject);
}


void CFlashlightComponent::PostInit(IGameObject * pGameObject)
{
	// We want to supply interaction verbs.
	m_interactor = static_cast<IEntityInteractionComponent*> (GetGameObject()->AcquireExtension("EntityInteraction"));
	if (m_interactor)
	{
		auto switchToggleInteractPtr = std::make_shared<CInteractionSwitchToggle>(this);
		m_interactor->AddInteraction(switchToggleInteractPtr);

		auto switchOnInteractPtr = std::make_shared<CInteractionSwitchOn>(this);
		m_interactor->AddInteraction(switchOnInteractPtr);

		auto switchOffInteractPtr = std::make_shared<CInteractionSwitchOff>(this);
		m_interactor->AddInteraction(switchOffInteractPtr);

		auto inspectInteractPtr = std::make_shared<CInteractionInspect>(this);
		m_interactor->AddInteraction(inspectInteractPtr);

		auto interactPtr = std::make_shared<CInteractionInteract>(this);
		m_interactor->AddInteraction(interactPtr);

		auto pickupInteractPtr = std::make_shared<CInteractionPickup>(this);
		m_interactor->AddInteraction(pickupInteractPtr);

		auto dropInteractPtr = std::make_shared<CInteractionDrop>(this);
		m_interactor->AddInteraction(dropInteractPtr);
	}

	// Need to init effects controllers if we want to use one.
	GetEffectsController().Init(GetEntityId());

	// Reset the entity.
	Reset();
}


void CFlashlightComponent::ProcessEvent(SEntityEvent& event)
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


// ***
// *** CFlashlightComponent
// ***


CFlashlightComponent::CFlashlightComponent()
{}


CFlashlightComponent::~CFlashlightComponent()
{
}


void CFlashlightComponent::Reset()
{
	// HACK: shouldn't do it at this time, making life easier for testing for now. Remove this!
	LoadFromXML();

	// TODO: should check for FP / TP e.g. when in inventory / hand.
	// NOTE: Weird stuff happens when flipping between FP and TP. The light doesn't get positioned in first person, and
	// it's rotated and positioned wrong in third person. Need to find the issue and fix it.
	m_fpGeomSlotId = GetEntity()->LoadCharacter(eIGS_ThirdPerson, "objects/FlashLight/FlashLight_Setup_v1/FlashLight.cdf");

	// Setup the physics parameters this instance should use.
	SEntityPhysicalizeParams PhysParams;
	PhysParams.type = PE_RIGID;
	PhysParams.mass = 1.5f;
	GetEntity()->Physicalize(PhysParams);

	// Check to see if the light should be on, and switch it on if needed.
	Switch(m_isSwitchedOn);
}


void CFlashlightComponent::GetSharedParameters(XmlNodeRef rootParams)
{
	m_itemFlashlightParameterShared = SharedParameters::GetSharedParameters<SItemFlashlightParameterShared>(
		rootParams, "item", GetEntity()->GetClass()->GetName(), "flashlight");
	m_dynamicLightParameterShared = SharedParameters::GetSharedParameters<SDynamicLight>(
		rootParams, "item", GetEntity()->GetClass()->GetName(), "dynamic_light");
}


void CFlashlightComponent::LoadFromXML()
{
	// We use the class name to determine which XML file contains our parameters. The item system scan means it's
	// location is already known.
	auto pItemSystem = gEnv->pGame->GetIGameFramework()->GetIItemSystem();
	auto xmlFilename = pItemSystem->GetItemParamsDescriptionFile(GetEntity()->GetClass()->GetName());
	XmlNodeRef rootParams = gEnv->pSystem->LoadXmlFromFile(xmlFilename);

	if (rootParams)
	{
		CItem::GetSharedParameters(rootParams);
		GetSharedParameters(rootParams);
	}
	else
	{
		GameWarning("Item description file %s doesn't exist for item %s", xmlFilename, GetEntity()->GetClass()->GetName());
	}
}


void CFlashlightComponent::ToggleSwitch()
{
	m_isSwitchedOn = !m_isSwitchedOn;
	Switch(m_isSwitchedOn);
}


void CFlashlightComponent::Switch(bool isSwitchedOn)
{
	m_isSwitchedOn = isSwitchedOn;

	if (isSwitchedOn)
	{
		// If we don't appear to have a light effect, then it's time to make one and attach it.
		if (m_lightId == EntityEffects::EFFECTID_INVALID)
			m_lightId = AttachLight(eIGS_FirstPerson, "light_term", Vec3(ZERO), Vec3(0.0f, 1.0f, 0.0f), eIGS_Last, m_dynamicLightParameterShared);
	}
	else
	{
		// Detach the light effect when the flashlight is switched off.
		if (m_lightId)
			DetachEffect(m_lightId);
		m_lightId = EntityEffects::EFFECTID_INVALID;
	}
}


void CFlashlightComponent::Register()
{
	// Register the class as a factory.
	CGameFactory::RegisterFactory<CFlashlightComponent>("Flashlight", false);
}
