#include <StdAfx.h>

#include "EntityInteractionComponent.h"
#include <CryCore/StaticInstanceList.h>
#include "CrySchematyc/Env/Elements/EnvComponent.h"
#include "CrySchematyc/Env/IEnvRegistrar.h"
#include "Components/Player/PlayerComponent.h"


namespace Chrysalis
{
static void RegisterEntityInteractionComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CEntityInteractionComponent));
		// Functions
		{
		}
	}
}


void CEntityInteractionComponent::ReflectType(Schematyc::CTypeDesc<CEntityInteractionComponent>& desc)
{
	desc.SetGUID(CEntityInteractionComponent::IID());
	desc.SetEditorCategory("Entities");
	desc.SetLabel("Entity Interaction");
	desc.SetDescription("Allow interaction with this entity.");
	desc.SetIcon("icons:ObjectTypes/light.ico");

	desc.SetComponentFlags({IEntityComponent::EFlags::Singleton});
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
	m_interactionQueue.push_back(interaction);
}


void CEntityInteractionComponent::RemoveInteraction(string verb)
{
	m_interactionQueue.erase(std::remove_if(m_interactionQueue.begin(), m_interactionQueue.end(),
		[&](IInteractionPtr i) { return i->GetVerb().compare(verb) == 0; }),
		m_interactionQueue.end());
}


std::vector<string> CEntityInteractionComponent::GetVerbs(bool includeHidden)
{
	std::vector<string> verbs;

	for (auto& it : m_interactionQueue)
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
	for (auto& it : m_interactionQueue)
	{
		if ((it->GetVerb().compare(verb) == 0) && (it->IsEnabled()))
		{
			return it;
		}
	}

	CryLogAlways("There's no interaction verb for %s", verb.c_str());

	return std::weak_ptr<IInteraction>();
}


IInteractionWeakPtr CEntityInteractionComponent::SelectInteractionVerb(string verb)
{
	for (auto& it : m_interactionQueue)
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


void CEntityInteractionComponent::OnInteractionStart(IActor& actor)
{
	CRY_ASSERT_MESSAGE(m_selectedInteraction, "Be sure to set an interaction before attempting to call it.");
	m_selectedInteraction->OnInteractionStart(actor);
}


void CEntityInteractionComponent::OnInteractionTick(IActor& actor)
{
	CRY_ASSERT_MESSAGE(m_selectedInteraction, "Be sure to set an interaction before attempting to call it.");
	m_selectedInteraction->OnInteractionTick(actor);
}


void CEntityInteractionComponent::OnInteractionComplete(IActor& actor)
{
	CRY_ASSERT_MESSAGE(m_selectedInteraction, "Be sure to set an interaction before attempting to call it.");
	m_selectedInteraction->OnInteractionComplete(actor);
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterEntityInteractionComponent)
}