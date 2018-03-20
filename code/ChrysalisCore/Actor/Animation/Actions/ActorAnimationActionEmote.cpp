#include "StdAfx.h"

#include "ActorAnimationActionEmote.h"

namespace Chrysalis
{
SMannequinEmoteParams g_emoteMannequinParams;


FragmentID CActorAnimationActionEmote::FindFragmentId(const SAnimationContext& context)
{
	const SMannequinEmoteParams* pUserParams = GetMannequinUserParams<SMannequinEmoteParams>(context);
	CRY_ASSERT(pUserParams != NULL);

	return pUserParams->fragmentIDs.Emote;
}


CActorAnimationActionEmote::CActorAnimationActionEmote(TagID emoteTagId) :
	CAnimationAction(EActorActionPriority::eAAP_Action, FRAGMENT_ID_INVALID, TAG_STATE_EMPTY)
{
	m_emoteTagId = emoteTagId;
}


void CActorAnimationActionEmote::OnInitialise()
{
	CAnimationAction::OnInitialise();

	m_emoteParams = GetMannequinUserParams<SMannequinEmoteParams>(*m_context);
	CRY_ASSERT(m_emoteParams);

	// Set the fragment for animation.
	SetFragment(m_emoteParams->fragmentIDs.Emote);
}


void CActorAnimationActionEmote::Enter()
{
	CAnimationAction::Enter();

	m_rootScope->GetActionController().GetContext().state.Set(m_emoteTagId, true);
}


void CActorAnimationActionEmote::Exit()
{
	CAnimationAction::Exit();

	m_rootScope->GetActionController().GetContext().state.Set(m_emoteTagId, false);
}


IAction::EStatus CActorAnimationActionEmote::Update(float timePassed)
{
	CAnimationAction::Update(timePassed);

	// Update the fragments and tags if they are different.
	const IScope& rootScope = GetRootScope();
	if (rootScope.IsDifferent(m_fragmentID, m_fragTags))
	{
		SetFragment(m_fragmentID, m_fragTags);
	}

	return m_eStatus;
}


bool CActorAnimationActionEmote::IsSupported(const SAnimationContext& context)
{
	const FragmentID fragmentId = FindFragmentId(context);
	const bool isSupported = (fragmentId != FRAGMENT_ID_INVALID);
	return isSupported;
}
}