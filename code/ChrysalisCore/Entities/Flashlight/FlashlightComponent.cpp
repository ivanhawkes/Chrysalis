#include <StdAfx.h>

#include "FlashlightComponent.h"
#include <GameXmlParamReader.h>
#include <Actor/Actor.h>
#include <Components/Interaction/EntityInteractionComponent.h>
#include <Components/Interaction/ItemInteractionComponent.h>
#include <Components/Snaplocks/SnaplockComponent.h>
#include <CryAnimation/ICryAnimation.h>


namespace Chrysalis
{
void CFlashlightComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CFlashlightComponent::ReflectType(Schematyc::CTypeDesc<CFlashlightComponent>& desc)
{
	desc.SetGUID(CFlashlightComponent::IID());
	desc.SetEditorCategory("Items");
	desc.SetLabel("Flashlight Component");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/item.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });

	desc.AddMember(&CFlashlightComponent::m_batteryLevel, 'batl', "BatteryLevel", "Battery Level", "Amount battery power left.", 1.0f);
	desc.AddMember(&CFlashlightComponent::m_isSwitchedOn, 'ison', "IsSwitchedOn", "Is Switched On?", "Is the light switched on?", false);
}


// ***
// *** IEntityComponent
// ***


void CFlashlightComponent::Initialize()
{
	const auto pEntity = GetEntity();

	m_pGeometryComponent = pEntity->CreateComponent<Cry::DefaultComponents::CStaticMeshComponent>();
	// TODO: Replace this functionality with new method from 5.4.
	//m_pGeometryComponent->AddEventListener(this);
	m_pDynamicLightComponent = pEntity->CreateComponent<CDynamicLightComponent>();
	// TODO: Replace this functionality with new method from 5.4.
	//m_pDynamicLightComponent->AddEventListener(this);

	// Standard item interactions.
	m_pItemInteractionComponent = pEntity->CreateComponent<CItemInteractionComponent>();

	// We want to supply interaction verbs.
	m_interactor = pEntity->GetOrCreateComponent<CEntityInteractionComponent>();
	if (m_interactor)
	{
		m_interactor->AddInteraction(std::make_shared<CInteractionSwitchToggle>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionSwitchOn>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionSwitchOff>(this));
		m_interactor->AddInteraction(std::make_shared<CInteractionInteract>(this));
	}

	// Manage our snaplocks.
	m_pSnaplockComponent = pEntity->GetOrCreateComponent<CSnaplockComponent>();
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_LEFTHAND, true));
	m_pSnaplockComponent->AddSnaplock(ISnaplock(SLT_ACTOR_RIGHTHAND, true));

	// Reset the entity.
	OnResetState();
}


// ***
// *** CFlashlightComponent
// ***


void CFlashlightComponent::OnResetState()
{
	// Check to see if the light should be on, and switch it on if needed.
	Switch(m_isSwitchedOn);
}


//void CFlashlightComponent::OnGeometryResetState()
//{
//	SEntitySlotInfo slotInfo;
//	const bool validSlot = GetEntity()->GetSlotInfo(m_pGeometryComponent->GetSlotId(), slotInfo);
//
//	if (validSlot)
//	{
//		if (slotInfo.pStatObj)
//		{
//			// Grab the position and rotation of the helper. Light rotation is X forward, so we need to adjust it
//			// for a Y forward scenario.
//			m_lightLocalPosition = slotInfo.pStatObj->GetHelperPos("light_term");
//			m_lightLocalRotation = kRightToForward * Quat(slotInfo.pStatObj->GetHelperTM("light_term")).GetNormalized();
//		}
//		else if (slotInfo.pCharacter)
//		{
//			IAttachmentManager *pAttachmentManager = slotInfo.pCharacter->GetIAttachmentManager();
//			IAttachment *pAttachment = pAttachmentManager->GetInterfaceByName("light_term");
//
//			if (pAttachment)
//			{
//				// #TODO: Get pos + rot for character.
//
//				// Grab the position and rotation of the helper. Light rotation is X forward, so we need to adjust it
//				// for a Y forward scenario.
//				// kRightToForward * something
//			}
//		}
//	}
//}
//
//
//void CFlashlightComponent::OnDynamicLightResetState()
//{
//	// Create a matrix for the scale, rotation and position of the light, locally.
//	Matrix34 localMatrix = Matrix34(Vec3 (1.0f, 1.0f, 1.0f), m_lightLocalRotation, m_lightLocalPosition);
//
//	// Move the light to the new position and rotation.
//	m_pDynamicLightComponent->SetLocalTM(localMatrix);
//}


//void CFlashlightComponent::GetSharedParameters(XmlNodeRef rootParams)
//{
//	m_itemFlashlightParameterShared = SharedParameters::GetSharedParameters<SItemFlashlightParameterShared>(
//		rootParams, "item", GetEntity()->GetClass()->GetName(), "flashlight");
//	m_dynamicLightParameterShared = SharedParameters::GetSharedParameters<SDynamicLight>(
//		rootParams, "item", GetEntity()->GetClass()->GetName(), "dynamic_light");
//}


//void CFlashlightComponent::LoadFromXML()
//{
//	// We use the class name to determine which XML file contains our parameters. The item system scan means it's
//	// location is already known.
//	auto pItemSystem = gEnv->pGameFramework->GetIItemSystem();
//	auto xmlFilename = pItemSystem->GetItemParamsDescriptionFile(GetEntity()->GetClass()->GetName());
//	XmlNodeRef rootParams = gEnv->pSystem->LoadXmlFromFile(xmlFilename);
//
//	if (rootParams)
//	{
//		CItemComponent::GetSharedParameters(rootParams);
//		GetSharedParameters(rootParams);
//	}
//	else
//	{
//		GameWarning("Item description file %s doesn't exist for item %s", xmlFilename, GetEntity()->GetClass()->GetName());
//	}
//}


void CFlashlightComponent::ToggleSwitch()
{
	Switch(!m_isSwitchedOn);
}


void CFlashlightComponent::Switch(bool isSwitchedOn)
{
	m_isSwitchedOn = isSwitchedOn;

	if (isSwitchedOn)
	{
		m_pDynamicLightComponent->SetActive(true);
		m_pDynamicLightComponent->OnResetState();
	}
	else
	{
		m_pDynamicLightComponent->SetActive(false);
		m_pDynamicLightComponent->OnResetState();
	}
}
}