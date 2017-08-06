#include "StdAfx.h"

#include "SwitchComponent.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


namespace Chrysalis
{
void CSwitchComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CSwitchComponent::ReflectType(Schematyc::CTypeDesc<CSwitchComponent>& desc)
{
	desc.SetGUID(CSwitchComponent::IID());
	desc.SetEditorCategory("Switch");
	desc.SetLabel("Switch");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });

	desc.AddMember(&CSwitchComponent::m_isEnabled, 'isen', "IsEnabled", "IsEnabled", "Is this switch currently enabled.", true);
	desc.AddMember(&CSwitchComponent::m_isSwitchedOn, 'ison', "SwitchedOn", "Switched On", "Is this switch currently switch on.", true);
	desc.AddMember(&CSwitchComponent::m_isSingleUseOnly, 'issi', "IsSingleUseOnly", "Single Use Only", "Is this switch only able to be used once.", true);
	// TODO: CRITICAL: HACK: BROKEN: !!
//	desc.AddMember(&CSwitchComponent::m_queueSignal, 'alts', "SwitchVerb", "Switch Verb (Override)", "Send an alternative queue signal to DRS if the string is not empty. ('interaction_switch').", "");
//	desc.AddMember(&CSwitchComponent::m_switchOnVerb, 'onve', "SwitchOnVerb", "Switch On Verb (Override)", "Send this verb to DRS instead of the default ('interaction_switch_on').", "");
//	desc.AddMember(&CSwitchComponent::m_switchOffVerb, 'offv', "SwitchOffVerb", "Switch Off Verb (Override)", "Send this verb to DRS instead of the default ('interaction_switch_off').", "");
}


void CSwitchComponent::Initialize()
{
	// Get some geometry.
	m_pGeometryComponent = GetEntity()->CreateComponent<Cry::DefaultComponents::CStaticMeshComponent>();

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

	// Notify listeners.
	for (auto& pListener : m_ListenersList)
	{
		pListener->OnSwitchResetState();
	}

	// #TODO: Should we call the SwitchOn / SwitchOff routines?
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
	}

	// Disable after a single use.
	if (m_isSingleUseOnly)
		m_isEnabled = false;
}


void CSwitchComponent::OnInteractionSwitchOn()
{
	if (m_isEnabled)
	{
		gEnv->pLog->LogAlways("SwitchOn fired.");
		m_isSwitchedOn = true;
		const string verb = m_switchOnVerb.IsEmpty() ? kSwitchOnVerb : m_switchOnVerb;
		InformAllLinkedEntities(verb, true);
	}

	// Disable after a single use.
	if (m_isSingleUseOnly)
		m_isEnabled = false;
}


void CSwitchComponent::OnInteractionSwitchOff()
{
	if (m_isEnabled)
	{
		gEnv->pLog->LogAlways("SwitchOff fired.");
		m_isSwitchedOn = false;
		const string verb = m_switchOffVerb.IsEmpty() ? kSwitchOffVerb : m_switchOffVerb;
		InformAllLinkedEntities(verb, true);
	}

	// Disable after a single use.
	if (m_isSingleUseOnly)
		m_isEnabled = false;
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
		const string queueSignalVerb = m_queueSignal.IsEmpty() ? kQueueSignal : m_queueSignal;
		pDrsProxy->GetResponseActor()->QueueSignal(queueSignalVerb, pContextVariableCollection);

		// Next please.
		entityLinks = entityLinks->next;
	}
}
}