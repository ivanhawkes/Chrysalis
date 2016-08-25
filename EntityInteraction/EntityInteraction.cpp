#include "StdAfx.h"

#include "EntityInteraction.h"
#include <Actor/Player/PlayerInput/IPlayerInput.h>
#include <Actor/Player/Player.h>


// ***
// *** IGameObjectExtension
// ***


void CEntityInteraction::PostInit(IGameObject * pGameObject)
{
	// Allow this instance to be updated every frame.
	pGameObject->EnableUpdateSlot(this, 0);
}


void CEntityInteraction::Update(SEntityUpdateContext& ctx, int updateSlot)
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
// *** IEntityInteraction
// ***


void IEntityInteraction::AddInteraction(IInteractionPtr interaction)
{
	m_Interactions.push_back(interaction);
}


void IEntityInteraction::RemoveInteraction(string verb)
{
	for (auto it = m_Interactions.begin(); it != m_Interactions.end(); ++it)
	{
		if (it->get()->GetVerb().compare(verb) == 0)
		{
			m_Interactions.erase(it);
			return;
		}
	}
}


std::vector<string> IEntityInteraction::GetVerbs(bool includeHidden)
{
	std::vector<string> verbs;

	for each (auto it in m_Interactions)
	{
		if (it.get()->IsEnabled())
		{
			if ((it.get()->IsHidden() == false) || ((it.get()->IsHidden() == true) && includeHidden))
			{
				verbs.push_back(it->GetVerb());
			}
		}
	}

	return verbs;
}


IInteractionWeakPtr IEntityInteraction::GetInteraction(string verb)
{
	for each (auto it in m_Interactions)
	{
		if ((it.get()->GetVerb().compare(verb) == 0) && (it.get()->IsEnabled()))
		{
			return it;
		}
	}

	return std::weak_ptr<IInteraction>();
}


IInteractionWeakPtr IEntityInteraction::SelectInteractionVerb(string verb)
{
	for each (auto it in m_Interactions)
	{
		if ((it.get()->GetVerb().compare(verb) == 0) && (it.get()->IsEnabled()))
		{
			m_selectedInteraction = it;
			return it;
		}
	}

	return std::weak_ptr<IInteraction>();
}


void IEntityInteraction::ClearInteractionVerb()
{
	m_selectedInteraction = IInteractionPtr();
}


//void IEntityInteraction::OnInit()
//{
//	if (m_selectedInteraction)
//	{
//		m_selectedInteraction->OnInit();
//	}
//}
//
//
//void IEntityInteraction::OnReset()
//{
//	if (m_selectedInteraction)
//	{
//		m_selectedInteraction->OnReset();
//	}
//}


void IEntityInteraction::OnInteractionStart()
{
	if (m_selectedInteraction)
	{
		m_selectedInteraction->OnInteractionStart();
	}
}


//void IEntityInteraction::OnUpdate(float deltaX, float deltaY)
//{
//	if (m_selectedInteraction)
//	{
//		m_selectedInteraction->OnUpdate(deltaX, deltaY);
//	}
//}


void IEntityInteraction::OnInteractionComplete()
{
	if (m_selectedInteraction)
	{
		m_selectedInteraction->OnInteractionComplete();
	}
}


void IEntityInteraction::OnInteractionCancel()
{
	if (m_selectedInteraction)
	{
		m_selectedInteraction->OnInteractionCancel();
	}
}


//void IEntityInteraction::OnPost()
//{
//	if (m_selectedInteraction)
//	{
//		m_selectedInteraction->OnPost();
//	}
//}


// ***
// *** CEntityInteraction
// ***

