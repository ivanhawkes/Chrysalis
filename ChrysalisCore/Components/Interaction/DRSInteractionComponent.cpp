#include "StdAfx.h"

#include "DRSInteractionComponent.h"
#include <Components/Interaction/EntityInteractionComponent.h>
#include <CrySerialization/Enum.h>
#include <CryDynamicResponseSystem/IDynamicResponseSystem.h>


class DRSInteractionComponentRegistrator : public IEntityRegistrator
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CDRSInteractionComponent>("DRSInteractionComponent", "Interaction", "physicsobject.bmp", true);

		// This should make the entity class invisible in the editor.
		//auto cls = gEnv->pEntitySystem->GetClassRegistry()->FindClass("DRSInteractionComponent");
		//cls->SetFlags(cls->GetFlags() | ECLF_INVISIBLE);
	}
};

DRSInteractionComponentRegistrator g_DRSInteractionComponentRegistrator;

CRYREGISTER_CLASS(CDRSInteractionComponent);


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


void CDRSInteractionComponent::SerializeProperties(Serialization::IArchive& archive)
{
	archive(m_drsResponse, "DRSResponse", "DRS Response (verb)");
	archive.doc("Verb to pass into DRS e.g. interaction_play_audio.");

	archive(m_drsProperties, "DRSProperties", "DRS Properties");

	if (archive.isInput())
	{
		OnResetState();
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
		for each (auto it in m_drsProperties)
		{
			pContextVariableCollection->CreateVariable(CHashedString(it.key), CHashedString(it.value));			
		}

		// Queue it and let the DRS handle it now.
		pDrsProxy->GetResponseActor()->QueueSignal(m_drsResponse, pContextVariableCollection);
	}
}
