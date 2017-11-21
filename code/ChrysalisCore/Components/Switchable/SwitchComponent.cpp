#include "StdAfx.h"

#include "SwitchComponent.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


namespace Chrysalis
{
static void ReflectType(Schematyc::CTypeDesc<CSwitchComponent::SSwitchOnSignal>& desc)
{
	desc.SetGUID("{D250B16F-D529-4A79-9268-FFBFA4252A80}"_cry_guid);
	desc.SetLabel("Switch On");
}


static void ReflectType(Schematyc::CTypeDesc<CSwitchComponent::SSwitchOffSignal>& desc)
{
	desc.SetGUID("{70C42CEE-ED6B-437C-A18D-C9441BBB6C99}"_cry_guid);
	desc.SetLabel("Switch Off");
}


static void ReflectType(Schematyc::CTypeDesc<CSwitchComponent::SSwitchToggleSignal>& desc)
{
	desc.SetGUID("{71497D0B-6600-4862-B8E1-29016D92E606}"_cry_guid);	
	desc.SetLabel("Switch Toggle");
}


void CSwitchComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CSwitchComponent::SSwitchOnSignal));
	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CSwitchComponent::SSwitchOffSignal));
	componentScope.Register(SCHEMATYC_MAKE_ENV_SIGNAL(CSwitchComponent::SSwitchToggleSignal));
}


void CSwitchComponent::ReflectType(Schematyc::CTypeDesc<CSwitchComponent>& desc)
{
	desc.SetGUID(CSwitchComponent::IID());
	desc.SetEditorCategory("Switch");
	desc.SetLabel("Interaction Switch");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::None });

	desc.AddMember(&CSwitchComponent::m_isEnabled, 'isen', "IsEnabled", "IsEnabled", "Is this switch currently enabled.", true);
	desc.AddMember(&CSwitchComponent::m_isSwitchedOn, 'ison', "SwitchedOn", "Switched On", "Is this switch currently switch on.", true);
	desc.AddMember(&CSwitchComponent::m_isSingleUseOnly, 'issi', "IsSingleUseOnly", "Single Use Only", "Is this switch only able to be used once.", false);
	desc.AddMember(&CSwitchComponent::m_queueSignal, 'alts', "SwitchVerb", "Switch Verb (Override)", "Send an alternative queue signal to DRS if the string is not empty. ('interaction_switch').", "");
}


void CSwitchComponent::Initialize()
{
	// We want to supply interaction verbs.
	m_interactor = GetEntity()->GetOrCreateComponent<CEntityInteractionComponent>();
	if (m_interactor)
	{
		m_switchTogglePtr = std::make_shared<CInteractionSwitchToggle>(this);
		m_interactor->AddInteraction(m_switchTogglePtr);

		m_switchOnPtr = std::make_shared<CInteractionSwitchOn>(this);
		m_interactor->AddInteraction(m_switchOnPtr);

		m_switchOffPtr = std::make_shared<CInteractionSwitchOff>(this);
		m_interactor->AddInteraction(m_switchOffPtr);
	}
}


void CSwitchComponent::OnResetState()
{
	m_switchTogglePtr->SetEnabled(m_isEnabled);
	m_switchOnPtr->SetEnabled(m_isEnabled);
	m_switchOffPtr->SetEnabled(m_isEnabled);
}


void CSwitchComponent::OnInteractionSwitchToggle()
{
	if (m_isEnabled)
	{
		gEnv->pLog->LogAlways("SwitchToggle fired.");
		if (m_isSwitchedOn)
			OnInteractionSwitchOff();
		else
			OnInteractionSwitchOn();

		// Push the signal out using schematyc.
		if (auto const pSchematycObject = GetEntity()->GetSchematycObject())
			pSchematycObject->ProcessSignal(SSwitchToggleSignal(), GetGUID());
	}
}


void CSwitchComponent::OnInteractionSwitchOn()
{
	if (m_isEnabled)
	{
		gEnv->pLog->LogAlways("SwitchOn fired.");
		m_isSwitchedOn = true;
		InformAllLinkedEntities(kSwitchOnVerb, true);

		// Push the signal out using schematyc.
		if (auto const pSchematycObject = GetEntity()->GetSchematycObject())
			pSchematycObject->ProcessSignal(SSwitchOnSignal(), GetGUID());

		// Disable after a single use.
		if (m_isSingleUseOnly)
			m_isEnabled = false;
	}
}


void CSwitchComponent::OnInteractionSwitchOff()
{
	if (m_isEnabled)
	{
		gEnv->pLog->LogAlways("SwitchOff fired.");
		m_isSwitchedOn = false;
		InformAllLinkedEntities(kSwitchOffVerb, true);

		// Push the signal out using schematyc.
		if (auto const pSchematycObject = GetEntity()->GetSchematycObject())
			pSchematycObject->ProcessSignal(SSwitchOffSignal(), GetGUID());

		// Disable after a single use.
		if (m_isSingleUseOnly)
			m_isEnabled = false;
	}
}


void CSwitchComponent::InformAllLinkedEntities(string verb, bool isSwitchedOn)
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

		// The switch value is always set, regardless of which verb was triggered.
		pContextVariableCollection->CreateVariable("IsSwitchedOn", isSwitchedOn);

		// Queue it and let the DRS handle it now.
		const string queueSignalVerb = m_queueSignal.empty() ? kQueueSignal : m_queueSignal.c_str();
		pDrsProxy->GetResponseActor()->QueueSignal(queueSignalVerb, pContextVariableCollection);

		// Next please.
		entityLinks = entityLinks->next;
	}
}
}