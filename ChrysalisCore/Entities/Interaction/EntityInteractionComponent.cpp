#include <StdAfx.h>

#include "EntityInteractionComponent.h"
#include <Player/Input/IPlayerInputComponent.h>
#include <Player/Player.h>


CRYREGISTER_CLASS(CEntityInteractionComponent)


class CEntityInteractionRegistrator
	: public IEntityRegistrator
	, public CEntityInteractionComponent::SExternalCVars
{
	virtual void Register() override
	{
		RegisterEntityWithDefaultComponent<CEntityInteractionComponent>("EntityInteraction", "Entities");

		// This should make the entity class invisible in the editor.
		auto cls = gEnv->pEntitySystem->GetClassRegistry()->FindClass("EntityInteraction");
		cls->SetFlags(cls->GetFlags() | ECLF_INVISIBLE);

		RegisterCVars();
	}

	void RegisterCVars()
	{
		REGISTER_CVAR2("entity_interaction_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
	}
};

CEntityInteractionRegistrator g_entityInteractionRegistrator;


// ***
// *** IEntityComponent
// ***


void CEntityInteractionComponent::Initialize()
{
	// Required for 5.3 to call update.
	GetEntity()->Activate(true);
}


void CEntityInteractionComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
		case ENTITY_EVENT_UPDATE:
			Update();
			break;
	}
}


void CEntityInteractionComponent::SerializeProperties(Serialization::IArchive& archive)
{
	//if (archive.isInput())
	//{
	//	OnResetState();
	//}
}


void CEntityInteractionComponent::Update()
{
	// #TODO: Remove this if it's not actually needed.
	auto pEntity = GetEntity();
	if (!pEntity)
		return;

	// NOTE: the thinking now is to just use the update on the extension instead.
	//if (m_selectedInteraction)
	//{
	//	auto pPlayer = CPlayer::GetLocalPlayer();
	//	if (pPlayer)
	//	{
	//		auto pPlayerInput = pPlayer->GetPlayerInput();
	//		m_selectedInteraction->OnUpdate(pPlayerInput->GetYawDelta(), pPlayerInput->GetPitchDelta());
	//	}
	//}
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
		[&] (IInteractionPtr i) { return i->GetVerb().compare(verb) == 0; }),
		m_Interactions.end());
}


std::vector<string> CEntityInteractionComponent::GetVerbs(bool includeHidden)
{
	std::vector<string> verbs;

	for (auto& it : m_Interactions)
	{
		if (it->IsEnabled())
		{
			if ((it->IsHidden() == false) || ((it->IsHidden() == true) && includeHidden))
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
	m_selectedInteraction = IInteractionPtr();
}


void CEntityInteractionComponent::OnInteractionStart()
{
	if (m_selectedInteraction)
	{
		m_selectedInteraction->OnInteractionStart();
	}
}


void CEntityInteractionComponent::OnInteractionComplete()
{
	if (m_selectedInteraction)
	{
		m_selectedInteraction->OnInteractionComplete();
	}
}


void CEntityInteractionComponent::OnInteractionCancel()
{
	if (m_selectedInteraction)
	{
		m_selectedInteraction->OnInteractionCancel();
	}
}


const CEntityInteractionComponent::SExternalCVars& CEntityInteractionComponent::GetCVars() const
{
	return g_entityInteractionRegistrator;
}
