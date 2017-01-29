#include "StdAfx.h"

#include "SwitchComponent.h"
#include <CrySerialization/Decorators/Resources.h>
#include <CrySerialization/Enum.h>
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


class CSwitchEntityRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CSwitchComponent>("SwitchComponent", "Switch", "physicsobject.bmp", true);
	}
};

CSwitchEntityRegistrator g_SwitchEntityRegistrator;

CRYREGISTER_CLASS(CSwitchComponent);


void CSwitchComponent::Initialize()
{
	// Get some geometry.
	m_pGeometryComponent = GetEntity()->GetOrCreateComponent<CGeometryComponent>();

	// We want to supply interaction verbs.
	auto m_interactor = GetEntity()->GetOrCreateComponent<CEntityInteractionComponent>();
	if (m_interactor)
	{
		m_switchTogglePtr = std::make_shared<CInteractionSwitchToggle>(this);
		m_interactor->AddInteraction(m_switchTogglePtr);

		m_switchOnPtr = std::make_shared<CInteractionSwitchOn>(this);
		m_interactor->AddInteraction(m_switchOnPtr);

		m_switchOffPtr = std::make_shared<CInteractionSwitchOff>(this);
		m_interactor->AddInteraction(m_switchOffPtr);
	}

	CDesignerEntityComponent::Initialize();
}


void CSwitchComponent::SerializeProperties(Serialization::IArchive& archive)
{
	archive(m_isEnabled, "IsEnabled", "Enabled");
	archive(m_isSwitchedOn, "SwitchedOn", "Switched On");
	archive(m_isSingleUseOnly, "IsSingleUseOnly", "Single Use Only");


	if (archive.isInput())
	{
		OnResetState();
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


void CSwitchComponent::SwitchToggle()
{
	if (m_isEnabled)
	{
		gEnv->pLog->LogAlways("SwitchToggle fired.");
		if (m_isSwitchedOn)
			SwitchOff();
		else
			SwitchOn();
	}

	// Disable after a single use.
	if (m_isSingleUseOnly)
		m_isEnabled = false;
}


void CSwitchComponent::SwitchOn()
{
	if (m_isEnabled)
	{
		gEnv->pLog->LogAlways("SwitchOn fired.");
		m_isSwitchedOn = true;
		InformAllLinkedEntities("interaction_switch_on", true);
	}

	// Disable after a single use.
	if (m_isSingleUseOnly)
		m_isEnabled = false;
}


void CSwitchComponent::SwitchOff()
{
	if (m_isEnabled)
	{
		gEnv->pLog->LogAlways("SwitchOff fired.");
		m_isSwitchedOn = false;
		InformAllLinkedEntities("interaction_switch_off", false);
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
		pDrsProxy->GetResponseActor()->QueueSignal("interaction_switch", pContextVariableCollection);

		// Next please.
		entityLinks = entityLinks->next;
	}
}
