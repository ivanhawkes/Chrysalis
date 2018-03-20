#include <StdAfx.h>

#include "EntityInteractionComponent.h"
#include "Components/Player/PlayerComponent.h"


namespace Chrysalis
{
void CEntityInteractionComponent::Register(Schematyc::CEnvRegistrationScope& componentScope)
{
}


void CEntityInteractionComponent::ReflectType(Schematyc::CTypeDesc<CEntityInteractionComponent>& desc)
{
	desc.SetGUID(CEntityInteractionComponent::IID());
	desc.SetEditorCategory("Entities");
	desc.SetLabel("Entity Interaction");
	desc.SetDescription("Allow interaction with this entity.");
	desc.SetIcon("icons:ObjectTypes/light.ico");

	desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton });
}


// ***
// *** IEntityComponent
// ***


void CEntityInteractionComponent::Initialize()
{
}


// ***
// *** CEntityInteractionComponent
// ***


void CEntityInteractionComponent::AddInteraction(IInteractionPtr interaction)
{
	m_Interactions.push_back(interaction);
}


void CEntityInteractionComponent::RemoveInteraction(string verb)
{
	m_Interactions.erase(std::remove_if(m_Interactions.begin(), m_Interactions.end(),
		[&](IInteractionPtr i) { return i->GetVerb().compare(verb) == 0; }),
		m_Interactions.end());
}


std::vector<string> CEntityInteractionComponent::GetVerbs(bool includeHidden)
{
	std::vector<string> verbs;

	for (auto& it : m_Interactions)
	{
		if (it->IsEnabled())
		{
			if ((!it->IsHidden()) || ((it->IsHidden()) && includeHidden))
			{
				verbs.push_back(it->GetVerb());
			}
		}
	}

	return verbs;
}


IInteractionWeakPtr CEntityInteractionComponent::GetInteraction(string verb)
{
	for (auto& it : m_Interactions)
	{
		if ((it->GetVerb().compare(verb) == 0) && (it->IsEnabled()))
		{
			return it;
		}
	}

	CryLogAlways("There's no interaction verb for %s", verb);

	return std::weak_ptr<IInteraction>();
}


IInteractionWeakPtr CEntityInteractionComponent::SelectInteractionVerb(string verb)
{
	for (auto& it : m_Interactions)
	{
		if ((it->GetVerb().compare(verb) == 0) && (it->IsEnabled()))
		{
			m_selectedInteraction = it;
			return it;
		}
	}

	return std::weak_ptr<IInteraction>();
}


void CEntityInteractionComponent::ClearInteractionVerb()
{
	m_selectedInteraction = nullptr;
}


void CEntityInteractionComponent::OnInteractionStart(IActorComponent& actor)
{
	CRY_ASSERT_MESSAGE(m_selectedInteraction, "Be sure to set an interaction before attempting to call it.");
	m_selectedInteraction->OnInteractionStart(actor);
}


void CEntityInteractionComponent::OnInteractionTick(IActorComponent& actor)
{
	CRY_ASSERT_MESSAGE(m_selectedInteraction, "Be sure to set an interaction before attempting to call it.");
	m_selectedInteraction->OnInteractionTick(actor);
}


void CEntityInteractionComponent::OnInteractionComplete(IActorComponent& actor)
{
	CRY_ASSERT_MESSAGE(m_selectedInteraction, "Be sure to set an interaction before attempting to call it.");
	m_selectedInteraction->OnInteractionComplete(actor);
}
}