#include "StdAfx.h"

#include "DRSInteractionComponent.h"
#include <Components/Interaction/EntityInteractionComponent.h>
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


namespace Chrysalis
{
void CDRSInteractionComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CDRSInteractionComponent::ReflectType(Schematyc::CTypeDesc<CDRSInteractionComponent>& desc)
{
	desc.SetGUID(CDRSInteractionComponent::IID());
	desc.SetEditorCategory("Interaction");
	desc.SetLabel("DRS Interaction");
	desc.SetDescription("No description.");
	desc.SetIcon("icons:ObjectTypes/light.ico");
	desc.SetComponentFlags({ IEntityComponent::EFlags::Transform });

//	desc.AddMember(&CDRSInteractionComponent::m_drsResponse, 'resp', "DRSResponse", "DRSResponse", "Verb to pass into DRS e.g. interaction_play_audio.", "");
//	desc.AddMember(&CDRSInteractionComponent::m_drsProperties, 'prop', "DRSProperties", "DRS Properties", "A list of properties to be passed to the DRS entity.", "");
}


void CDRSInteractionComponent::Initialize()
{
	// Add new verbs to the interactor.
	if (m_interactor = GetEntity()->GetOrCreateComponent<CEntityInteractionComponent>())
	{
		m_interactor->AddInteraction(std::make_shared<CInteractionDRS>(this));
	}

	// Reset the entity.
	OnResetState();
}


void CDRSInteractionComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		// Start the level.
		case ENTITY_EVENT_START_LEVEL:

			// Properties might have changed.
		case ENTITY_EVENT_RESET:
		case ENTITY_EVENT_EDITOR_PROPERTY_CHANGED:
		case ENTITY_EVENT_XFORM_FINISHED_EDITOR:
			OnResetState();
			break;
	}
}


void CDRSInteractionComponent::OnResetState()
{
}


void CDRSInteractionComponent::OnInteractionDRS()
{
	if (!m_drsResponse.IsEmpty())
	{
		// Default to self as the target entity.
		auto pTargetEntity = GetEntity();

		// Make sure the target entity has a DRS proxy.
		auto pDrsProxy = crycomponent_cast<IEntityDynamicResponseComponent*> (pTargetEntity->CreateProxy(ENTITY_PROXY_DYNAMICRESPONSE));

		// Create a context variable collection and populate it based on information from the target entity.
		DRS::IVariableCollectionSharedPtr pContextVariableCollection = gEnv->pDynamicResponseSystem->CreateContextCollection();

		// It might be useful to know which verb triggered the interaction.
		pContextVariableCollection->CreateVariable("Verb", CHashedString(m_drsResponse));

		// Add each key, value to the DRS variable collection.
		for (auto it : m_drsProperties)
		{
			pContextVariableCollection->CreateVariable(CHashedString(it.key), CHashedString(it.value));
		}

		// Queue it and let the DRS handle it now.
		pDrsProxy->GetResponseActor()->QueueSignal(m_drsResponse, pContextVariableCollection);
	}
}
}