#include "StdAfx.h"

#include "InteractComponent.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>
#include <Components/Player/Input/PlayerInputComponent.h>


namespace Chrysalis
{
static void ReflectType(Schematyc::CTypeDesc<CInteractComponent::SInteractStartSignal>& desc)
{
	desc.SetGUID("{D7834D96-13FB-41C4-90D1-F3D977CA0AC7}"_cry_guid);
	desc.SetLabel("Interact Start");
}


static void ReflectType(Schematyc::CTypeDesc<CInteractComponent::SInteractTickSignal>& desc)
{
	desc.SetGUID("{C11053C3-0CB4-4316-A643-F53BECBA07B5}"_cry_guid);
	desc.SetLabel("Interact Tick");
	desc.AddMember(&CInteractComponent::SInteractTickSignal::m_deltaPitch, 'dpit', "DeltaPitch", "Delta Pitch", "Player input requested change in pitch.", 0.0f);
	desc.AddMember(&CInteractComponent::SInteractTickSignal::m_deltaYaw, 'dyaw', "DeltaYaw", "Delta Yaw", "Player input requested change in yaw.", 0.0f);
}


static void ReflectType(Schematyc::CTypeDesc<CInteractComponent::SInteractCompleteSignal>& desc)
{
	desc.SetGUID("{6E153DFF-B21B-4E92-8D50-976B17802556}"_cry_guid);
	desc.SetLabel("Interact Complete");
}


static void ReflectType(Schematyc::CTypeDesc<CInteractComponent::SInteractCancelSignal>& desc)
{
	desc.SetGUID("{42532226-2D75-4AA6-92C4-AE4EB8DC5596}"_cry_guid);	
	desc.SetLabel("Interact Cancel");
}


void CInteractComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CInteractComponent::SInteractStartSignal));
	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CInteractComponent::SInteractTickSignal));
	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CInteractComponent::SInteractCompleteSignal));
	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CInteractComponent::SInteractCancelSignal));
}


void CInteractComponent::ReflectType(Schematyc::CTypeDesc<CInteractComponent>& desc)
{
	desc.SetGUID(CInteractComponent::IID());
	desc.SetEditorCategory("Interact");
	desc.SetLabel("Generic Interaction");
	desc.SetDescription("An interaction between a player and an entity.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::None });

	desc.AddMember(&CInteractComponent::m_isEnabled, 'isen', "IsEnabled", "IsEnabled", "Is this interaction currently enabled.", true);
	desc.AddMember(&CInteractComponent::m_isSingleUseOnly, 'issi', "IsSingleUseOnly", "Single Use Only", "Is this Interact only able to be used once.", false);
	desc.AddMember(&CInteractComponent::m_queueSignal, 'alts', "InteractVerb", "Interact Verb (Override)", "Send an alternative queue signal to DRS if the string is not empty. ('interaction_Interact').", "");
}


void CInteractComponent::Initialize()
{
	// We want to supply interaction verbs.
	m_interactor = GetEntity()->GetOrCreateComponent<CEntityInteractionComponent>();
	if (m_interactor)
	{
		m_interactPtr = std::make_shared<CInteractionInteract>(this);
		m_interactor->AddInteraction(m_interactPtr);
	}
}


void CInteractComponent::OnResetState()
{
	m_interactPtr->SetEnabled(m_isEnabled);
}


void CInteractComponent::OnInteractionInteractStart()
{
	if (m_isEnabled)
	{
		gEnv->pLog->LogAlways("OnInteractionInteractStart fired.");
		InformAllLinkedEntities(kInteractStartVerb, true);

		// Push the signal out using schematyc.
		if (auto const pSchematycObject = GetEntity()->GetSchematycObject())
			pSchematycObject->ProcessSignal(SInteractStartSignal(), GetGUID());

		// Disable after a single use.
		if (m_isSingleUseOnly)
			m_isEnabled = false;
	}
}


void CInteractComponent::OnInteractionInteractTick()
{
	if (m_isEnabled)
	{
		gEnv->pLog->LogAlways("OnInteractionInteractTick fired.");
		InformAllLinkedEntities(kInteractTickVerb, true);

		SInteractTickSignal interactTickSignal;

		// Pass along the delta movement for input devices. This helps enable interactive contol of entities.
		if (auto pPlayerInput = CPlayerComponent::GetLocalPlayer()->GetPlayerInput())
		{
			interactTickSignal.m_deltaPitch = pPlayerInput->GetPitchDelta();
			interactTickSignal.m_deltaYaw = pPlayerInput->GetYawDelta();
		}

		// Push the signal out using schematyc.
		if (auto const pSchematycObject = GetEntity()->GetSchematycObject())
			pSchematycObject->ProcessSignal(interactTickSignal, GetGUID());
	}
}


void CInteractComponent::OnInteractionInteractComplete()
{
	if (m_isEnabled)
	{
		gEnv->pLog->LogAlways("OnInteractionInteractComplete fired.");
		InformAllLinkedEntities(kInteractCompleteVerb, true);

		// Push the signal out using schematyc.
		if (auto const pSchematycObject = GetEntity()->GetSchematycObject())
			pSchematycObject->ProcessSignal(SInteractCompleteSignal(), GetGUID());
	}
}


void CInteractComponent::OnInteractionInteractCancel()
{
	if (m_isEnabled)
	{
		gEnv->pLog->LogAlways("OnInteractionInteractCancel fired.");
		InformAllLinkedEntities(kInteractCancelVerb, true);

		// Push the signal out using schematyc.
		if (auto const pSchematycObject = GetEntity()->GetSchematycObject())
			pSchematycObject->ProcessSignal(SInteractCompleteSignal(), GetGUID());
	}
}


void CInteractComponent::InformAllLinkedEntities(string verb, bool isInteractedOn)
{
	// Notify every linked entity.
	auto* entityLinks = GetEntity()->GetEntityLinks();
	while (entityLinks)
	{
		auto pTargetEntity = gEnv->pEntitySystem->GetEntity(entityLinks->entityId);
		auto pDrsProxy = crycomponent_cast<IEntityDynamicResponseComponent*> (pTargetEntity->CreateProxy(ENTITY_PROXY_DYNAMICRESPONSE));

		// Create a context variable collection and populate it based on information from the target entity.
		DRS::IVariableCollectionSharedPtr pContextVariableCollection = gEnv->pDynamicResponseSystem->CreateContextCollection();

		// It might be useful to know which verb triggered the interaction.
		pContextVariableCollection->CreateVariable("Verb", CHashedString(verb));

		// The Interact value is always set, regardless of which verb was triggered.
		pContextVariableCollection->CreateVariable("IsInteractedOn", isInteractedOn);

		// Queue it and let the DRS handle it now.
		const string queueSignalVerb = m_queueSignal.empty() ? kQueueSignal : m_queueSignal.c_str();
		pDrsProxy->GetResponseActor()->QueueSignal(queueSignalVerb, pContextVariableCollection);

		// Next please.
		entityLinks = entityLinks->next;
	}
}
}