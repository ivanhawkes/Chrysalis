#include "StdAfx.h"

#include "SwitchComponent.h"
#include "Actor/ActorComponent.h"
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


namespace Chrysalis
{
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
	desc.AddMember(&CSwitchComponent::m_isSingleUseOnly, 'issi', "IsSingleUseOnly", "Single Use Only", "Is this switch only able to be used once.", false);
	desc.AddMember(&CSwitchComponent::m_queueSignal, 'alts', "SwitchVerb", "Switch Verb (Override)", "Send an alternative queue signal to DRS if the string is not empty. ('interaction_switch').", "");
	desc.AddMember(&CSwitchComponent::m_isSwitchedOn, 'ison', "SwitchedOn", "Switched On", "Is this switch currently switch on.", true);

	// Mark the entity interaction component as a hard requirement.
	desc.AddComponentInteraction(SEntityComponentRequirements::EType::HardDependency, CEntityInteractionComponent::IID());
}


void CSwitchComponent::Initialize()
{
	//CInteractComponent::Initialize();

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


void CSwitchComponent::OnInteractionSwitchToggle(IInteraction& pInteraction, IActorComponent& actor)
{
	CInteractComponent::OnInteractionInteractStart(pInteraction, actor);
	
	if (m_isEnabled)
	{
		CryLogAlways("SwitchToggle fired.");
		if (m_isSwitchedOn)
			OnInteractionSwitchOff(pInteraction, actor);
		else
			OnInteractionSwitchOn(pInteraction, actor);

		// Push the signal out using schematyc.
		//if (auto const pSchematycObject = GetEntity()->GetSchematycObject())
		//	pSchematycObject->ProcessSignal(SSwitchToggleSignal(), GetGUID());
	}

	// Inform the actor we are finished with an interaction.
	//actor.InteractionEnd(&pInteraction);
}


void CSwitchComponent::OnInteractionSwitchOn(IInteraction& pInteraction, IActorComponent& actor)
{
	if (m_isEnabled)
	{
		CryLogAlways("SwitchOn fired.");
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


void CSwitchComponent::OnInteractionSwitchOff(IInteraction& pInteraction, IActorComponent& actor)
{
	if (m_isEnabled)
	{
		CryLogAlways("SwitchOff fired.");
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


// TODO: FIX: Simplify this so it doesn't all need to be replicated in derived classes such as this one.

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