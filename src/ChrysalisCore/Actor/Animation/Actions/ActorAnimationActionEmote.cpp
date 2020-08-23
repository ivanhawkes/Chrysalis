#include "StdAfx.h"

#include "ActorAnimationActionEmote.h"

namespace Chrysalis
{
// TODO: This really needs to be done with a case statement and hashes of the strings.
TagID CActorAnimationActionEmote::GetEmoteTagId(const string emoteName)
{
	auto emoteStr = string(emoteName).MakeLower();
	TagID emoteTagId {TAG_ID_INVALID};

	if (strcmp(emoteStr, "anger") == 0) emoteTagId = m_emoteParams->tagIDs.Anger;
	else if (strcmp(emoteStr, "annoyance") == 0) emoteTagId = m_emoteParams->tagIDs.Annoyance;
	else if (strcmp(emoteStr, "anxiety") == 0) emoteTagId = m_emoteParams->tagIDs.Anxiety;
	else if (strcmp(emoteStr, "apathy") == 0) emoteTagId = m_emoteParams->tagIDs.Apathy;
	else if (strcmp(emoteStr, "awe") == 0) emoteTagId = m_emoteParams->tagIDs.Awe;
	else if (strcmp(emoteStr, "boredom") == 0) emoteTagId = m_emoteParams->tagIDs.Boredom;
	else if (strcmp(emoteStr, "confidence") == 0) emoteTagId = m_emoteParams->tagIDs.Confidence;
	else if (strcmp(emoteStr, "contempt") == 0) emoteTagId = m_emoteParams->tagIDs.Contempt;
	else if (strcmp(emoteStr, "contentment") == 0) emoteTagId = m_emoteParams->tagIDs.Contentment;
	else if (strcmp(emoteStr, "courage") == 0) emoteTagId = m_emoteParams->tagIDs.Courage;
	else if (strcmp(emoteStr, "curiosity") == 0) emoteTagId = m_emoteParams->tagIDs.Curiosity;
	else if (strcmp(emoteStr, "depression") == 0) emoteTagId = m_emoteParams->tagIDs.Depression;
	else if (strcmp(emoteStr, "despair") == 0) emoteTagId = m_emoteParams->tagIDs.Despair;
	else if (strcmp(emoteStr, "disappointment") == 0) emoteTagId = m_emoteParams->tagIDs.Disappointment;
	else if (strcmp(emoteStr, "disgust") == 0) emoteTagId = m_emoteParams->tagIDs.Disgust;
	else if (strcmp(emoteStr, "dread") == 0) emoteTagId = m_emoteParams->tagIDs.Dread;
	else if (strcmp(emoteStr, "embarrassment") == 0) emoteTagId = m_emoteParams->tagIDs.Embarrassment;
	else if (strcmp(emoteStr, "excitement") == 0) emoteTagId = m_emoteParams->tagIDs.Excitement;
	else if (strcmp(emoteStr, "fear") == 0) emoteTagId = m_emoteParams->tagIDs.Fear;
	else if (strcmp(emoteStr, "frustration") == 0) emoteTagId = m_emoteParams->tagIDs.Frustration;
	else if (strcmp(emoteStr, "gratitude") == 0) emoteTagId = m_emoteParams->tagIDs.Gratitude;
	else if (strcmp(emoteStr, "grief") == 0) emoteTagId = m_emoteParams->tagIDs.Grief;
	else if (strcmp(emoteStr, "guilt") == 0) emoteTagId = m_emoteParams->tagIDs.Guilt;
	else if (strcmp(emoteStr, "happiness") == 0) emoteTagId = m_emoteParams->tagIDs.Happiness;
	else if (strcmp(emoteStr, "hatred") == 0) emoteTagId = m_emoteParams->tagIDs.Hatred;
	else if (strcmp(emoteStr, "hope") == 0) emoteTagId = m_emoteParams->tagIDs.Hope;
	else if (strcmp(emoteStr, "horror") == 0) emoteTagId = m_emoteParams->tagIDs.Horror;
	else if (strcmp(emoteStr, "hostility") == 0) emoteTagId = m_emoteParams->tagIDs.Hostility;
	else if (strcmp(emoteStr, "hurt") == 0) emoteTagId = m_emoteParams->tagIDs.Hurt;
	else if (strcmp(emoteStr, "hysteria") == 0) emoteTagId = m_emoteParams->tagIDs.Hysteria;
	else if (strcmp(emoteStr, "indifference") == 0) emoteTagId = m_emoteParams->tagIDs.Indifference;
	else if (strcmp(emoteStr, "interest") == 0) emoteTagId = m_emoteParams->tagIDs.Interest;
	else if (strcmp(emoteStr, "joy") == 0) emoteTagId = m_emoteParams->tagIDs.Joy;
	else if (strcmp(emoteStr, "loathing") == 0) emoteTagId = m_emoteParams->tagIDs.Loathing;
	else if (strcmp(emoteStr, "loneliness") == 0) emoteTagId = m_emoteParams->tagIDs.Loneliness;
	else if (strcmp(emoteStr, "outrage") == 0) emoteTagId = m_emoteParams->tagIDs.Outrage;
	else if (strcmp(emoteStr, "panic") == 0) emoteTagId = m_emoteParams->tagIDs.Panic;
	else if (strcmp(emoteStr, "pity") == 0) emoteTagId = m_emoteParams->tagIDs.Pity;
	else if (strcmp(emoteStr, "pleasure") == 0) emoteTagId = m_emoteParams->tagIDs.Pleasure;
	else if (strcmp(emoteStr, "pride") == 0) emoteTagId = m_emoteParams->tagIDs.Pride;
	else if (strcmp(emoteStr, "rage") == 0) emoteTagId = m_emoteParams->tagIDs.Rage;
	else if (strcmp(emoteStr, "regret") == 0) emoteTagId = m_emoteParams->tagIDs.Regret;
	else if (strcmp(emoteStr, "relief") == 0) emoteTagId = m_emoteParams->tagIDs.Relief;
	else if (strcmp(emoteStr, "remorse") == 0) emoteTagId = m_emoteParams->tagIDs.Remorse;
	else if (strcmp(emoteStr, "sadness") == 0) emoteTagId = m_emoteParams->tagIDs.Sadness;
	else if (strcmp(emoteStr, "satisfaction") == 0) emoteTagId = m_emoteParams->tagIDs.Satisfaction;
	else if (strcmp(emoteStr, "shame") == 0) emoteTagId = m_emoteParams->tagIDs.Shame;
	else if (strcmp(emoteStr, "shock") == 0) emoteTagId = m_emoteParams->tagIDs.Shock;
	else if (strcmp(emoteStr, "shyness") == 0) emoteTagId = m_emoteParams->tagIDs.Shyness;
	else if (strcmp(emoteStr, "sorrow") == 0) emoteTagId = m_emoteParams->tagIDs.Sorrow;
	else if (strcmp(emoteStr, "surprise") == 0) emoteTagId = m_emoteParams->tagIDs.Surprise;
	else if (strcmp(emoteStr, "terror") == 0) emoteTagId = m_emoteParams->tagIDs.Terror;
	else if (strcmp(emoteStr, "trust") == 0) emoteTagId = m_emoteParams->tagIDs.Trust;
	else if (strcmp(emoteStr, "wonder") == 0) emoteTagId = m_emoteParams->tagIDs.Wonder;
	else if (strcmp(emoteStr, "worry") == 0) emoteTagId = m_emoteParams->tagIDs.Worry;
	else if (strcmp(emoteStr, "zeal") == 0) emoteTagId = m_emoteParams->tagIDs.Zeal;
	else if (strcmp(emoteStr, "zest") == 0) emoteTagId = m_emoteParams->tagIDs.Zest;

	return emoteTagId;
}


FragmentID CActorAnimationActionEmote::FindFragmentId(const SAnimationContext& context)
{
	const SMannequinEmoteParams* pUserParams = GetMannequinUserParams<SMannequinEmoteParams>(context);
	CRY_ASSERT(pUserParams != nullptr);

	return pUserParams->fragmentIDs.Emote;
}


CActorAnimationActionEmote::CActorAnimationActionEmote(TagID emoteTagId) :
	CAnimationAction(EActorActionPriority::eAAP_Action, FRAGMENT_ID_INVALID, TAG_STATE_EMPTY)
{
	m_emoteTagId = emoteTagId;
}


CActorAnimationActionEmote::CActorAnimationActionEmote(string emoteTag) :
	CAnimationAction(EActorActionPriority::eAAP_Action, FRAGMENT_ID_INVALID, TAG_STATE_EMPTY)
{
	m_emoteTag = emoteTag;
}


void CActorAnimationActionEmote::OnInitialise()
{
	CAnimationAction::OnInitialise();

	m_emoteParams = GetMannequinUserParams<SMannequinEmoteParams>(*m_context);
	CRY_ASSERT(m_emoteParams);

	// Set the fragment for animation.
	SetFragment(m_emoteParams->fragmentIDs.Emote);

	// Get the emote tag ID if needed.
	if (m_emoteTagId == TAG_ID_INVALID)
	{
		m_emoteTagId = GetEmoteTagId(m_emoteTag);
	}
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
		SetFragment(m_fragmentID, m_fragTags);

	return m_eStatus;
}


bool CActorAnimationActionEmote::IsSupported(const SAnimationContext& context)
{
	const FragmentID fragmentId = FindFragmentId(context);
	const bool isSupported = (fragmentId != FRAGMENT_ID_INVALID);

	return isSupported;
}
}