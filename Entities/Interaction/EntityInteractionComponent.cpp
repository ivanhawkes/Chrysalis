#include <StdAfx.h>

#include "EntityInteractionComponent.h"
#include <Player/Input/IPlayerInputComponent.h>
#include <Player/Player.h>


class CEntityInteractionRegistrator
	: public IEntityRegistrator
	, public CEntityInteractionComponent::SExternalCVars
{
	virtual void Register() override
	{
		CGameFactory::RegisterGameObjectExtension<CEntityInteractionComponent>("EntityInteraction");

		RegisterCVars();
	}

	void RegisterCVars()
	{
		REGISTER_CVAR2("entity_interaction_Debug", &m_debug, 0, VF_CHEAT, "Allow debug display.");
	}
};

CEntityInteractionRegistrator g_entityInteractionRegistrator;


// ***
// *** IGameObjectExtension
// ***


void CEntityInteractionComponent::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);
}


void CEntityInteractionComponent::Update(SEntityUpdateContext& ctx, int updateSlot)
{
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
// *** IEntityInteractionComponent
// ***


void IEntityInteractionComponent::AddInteraction(IInteractionPtr interaction)
{
	m_Interactions.push_back(interaction);
}


void IEntityInteractionComponent::RemoveInteraction(string verb)
{
	m_Interactions.erase(std::remove_if(m_Interactions.begin(), m_Interactions.end(),
		[&] (IInteractionPtr i) { return i->GetVerb().compare(verb) == 0; }),
		m_Interactions.end());
}


std::vector<string> IEntityInteractionComponent::GetVerbs(bool includeHidden)
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


IInteractionWeakPtr IEntityInteractionComponent::GetInteraction(string verb)
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


IInteractionWeakPtr IEntityInteractionComponent::SelectInteractionVerb(string verb)
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


void IEntityInteractionComponent::ClearInteractionVerb()
{
	m_selectedInteraction = IInteractionPtr();
}


//void IEntityInteractionComponent::OnInit()
//{
//	if (m_selectedInteraction)
//	{
//		m_selectedInteraction->OnInit();
//	}
//}
//
//
//void IEntityInteractionComponent::OnReset()
//{
//	if (m_selectedInteraction)
//	{
//		m_selectedInteraction->OnReset();
//	}
//}


void IEntityInteractionComponent::OnInteractionStart()
{
	if (m_selectedInteraction)
	{
		m_selectedInteraction->OnInteractionStart();
	}
}


//void IEntityInteractionComponent::OnUpdate(float deltaX, float deltaY)
//{
//	if (m_selectedInteraction)
//	{
//		m_selectedInteraction->OnUpdate(deltaX, deltaY);
//	}
//}


void IEntityInteractionComponent::OnInteractionComplete()
{
	if (m_selectedInteraction)
	{
		m_selectedInteraction->OnInteractionComplete();
	}
}


void IEntityInteractionComponent::OnInteractionCancel()
{
	if (m_selectedInteraction)
	{
		m_selectedInteraction->OnInteractionCancel();
	}
}


//void IEntityInteractionComponent::OnPost()
//{
//	if (m_selectedInteraction)
//	{
//		m_selectedInteraction->OnPost();
//	}
//}


// ***
// *** CEntityInteractionComponent
// ***


const CEntityInteractionComponent::SExternalCVars& CEntityInteractionComponent::GetCVars() const
{
	return g_entityInteractionRegistrator;
}
